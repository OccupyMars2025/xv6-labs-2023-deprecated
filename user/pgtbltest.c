#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

void ugetpid_test();
void pgaccess_test();

int
main(int argc, char *argv[])
{
  ugetpid_test();
  pgaccess_test();
  printf("pgtbltest: all tests succeeded\n");
  exit(0);
}

char *testname = "???";

void
err(char *why)
{
  printf("pgtbltest: %s failed: %s, pid=%d\n", testname, why, getpid());
  exit(1);
}

void
ugetpid_test()
{
  int i;

  printf("ugetpid_test starting\n");
  testname = "ugetpid_test";

  for (i = 0; i < 64; i++) {
    int ret = fork();
    if (ret != 0) {
      wait(&ret);
      if (ret != 0)
        exit(1);
      continue;
    }
    if (getpid() != ugetpid())
      err("missmatched PID");
    exit(0);
  }
  printf("ugetpid_test: OK\n");
}

void
pgaccess_test()
{
  char *buf;
  uint64 abits; // array of bits, 64 bits, one bit for one page
  printf("pgaccess_test starting\n");
  testname = "pgaccess_test";
  // buf is a user virtual address, but is it page-aligned ?
  buf = malloc(32 * PGSIZE); 
  if (pgaccess(buf, 32, &abits) < 0)
    err("pgaccess failed");
  buf[PGSIZE * 1] += 1;
  buf[PGSIZE * 2] += 1;
  buf[PGSIZE * 30] += 1;
  if (pgaccess(buf, 32, &abits) < 0)
    err("pgaccess failed");
  if (abits != ((1 << 1) | (1 << 2) | (1 << 30)))
    err("incorrect access bits set");
  
  // for(int i=0; i<32; ++i) {
  //   for(int j=0; j<32; ++j) {
  //     buf[PGSIZE * i] += 2;
  //     buf[PGSIZE * j] += 3;
  //     if (pgaccess(buf, 32, &abits) < 0)
  //       err("pgaccess failed");
  //     if (abits != ((1 << i) | (1 << j)))
  //       err("incorrect access bits set");
  //   }
  // }
  free(buf);
  printf("pgaccess_test: OK\n");
}
