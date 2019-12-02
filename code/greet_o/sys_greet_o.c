#include <linux/kernel.h>   // For printk
#include <linux/string.h>   // For strcpy, strcat, strlen
#include <linux/uaccess.h>  // For copy_from_user, copy_to_user
#include <linux/syscalls.h> // For SYSCALL_DEFINE4

// Definition of the system call
SYSCALL_DEFINE4(greet_o,
                const char __user *, str,     // Input name
                const unsigned int, str_len,  // Length of input name
                char __user *, buf,           // Output buffer
                unsigned int, buf_len)        // Length of output buffer
{
  char name[64];    // kernel stack variable hold input name
  char message[96]; // kernel stack variable hold output message
  printk("greet_o system call invoked.\n");
  if (str_len >= 64)
  {
    printk("Is it a name or entire dictionary?\n");
    return -1;
  }
  // Copy data from user space into kernel space
  if (copy_from_user(name, str, str_len))
  {
    printk("Could not copy name into kernel space.\n");
    return -2;
  }
  // Build the final message
  strcpy(message, "Namaste ");
  strcat(message, name);
  strcat(message, "!");
  // Check if the final message can be fit into the output binary
  if (strlen(message) >= (buf_len - 1))
  {
    printk("Output buffer too small.\n");
    return -3;
  }
  // Copy back the message from the kernel space to the user space
  if (copy_to_user(buf, message, strlen(message) + 1))
  {
    printk("Could not copy message to user space.\n");
    return -4;
  }
  // Print the sent message into the kernel log
  printk("greet_o system call message: %s\n", message);
  return 0;
}
