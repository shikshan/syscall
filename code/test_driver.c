#define _GNU_SOURCE // for using non-POSIX api
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h> // This is not part of POSIX!

#define SYS_greet_o 999 // Hex - 0x3e7

int main(int argc, char **argv)
{
  char str[20] = "Omkar";
  char message[64] = "";
  // Make the greet_o system call
  int ret_val = syscall(SYS_greet_o, str, 6, message, 64);
  if (ret_val < 0)
  {
    printf("[ERR] Ret val: %d\n", ret_val);
    return 1;
  }
  printf("greet_o system call message: %s\n", message);
  return 0;
}
