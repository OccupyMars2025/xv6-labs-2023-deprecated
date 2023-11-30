#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
uint64
sys_pgaccess(void)
{
  uint64 user_base_address;
  argaddr(0, &user_base_address);
  // if((user_base_address % PGSIZE) != 0) {
  //   printf("Error: the user base address is NOT page-aligned\n");
  //   return -1;
  // }
  user_base_address = PGROUNDDOWN(user_base_address);

  uint64 bitmask = 0;
  int num_pages;
  argint(1, &num_pages);
  if(num_pages < 1 || num_pages > sizeof(bitmask)*8) {
    printf("Error: the scanned page number must be in [1, %d]\n", sizeof(bitmask)*8);
    return -1;
  }

  uint64 user_bitmask_address;
  argaddr(2, &user_bitmask_address);

  struct proc *process = myproc();
  pte_t *pte;
  for(int i = 0; i < num_pages; ++i) {
    pte = walk(process->pagetable, user_base_address + i * PGSIZE, 0);
    if(0 == pte) {
      return -1;
    }
    if(((*pte) & PTE_V) != 0 && ((*pte) & PTE_A) != 0) {
      bitmask |= 1 << i;
      (*pte) &= ~PTE_A;
    }
  }

  if(copyout(process->pagetable, user_bitmask_address, (char*)(&bitmask), sizeof(bitmask)) < 0) {
    return -1;
  }

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
