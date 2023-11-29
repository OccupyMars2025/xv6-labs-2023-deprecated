#include "kernel/types.h"
#include "user/user.h"

  int sum_to(int n) {
    int acc = 0;
    for (int i = 0; i <= n; i++) {
      acc += i;
    }
    return acc;
  }

// int 
// sum_then_double(int n) {
//   return 2* sum_to(n);
// }

int main() {
  sum_to(10);
  // sum_then_double(7);
  exit(0);
}
