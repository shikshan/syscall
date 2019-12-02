# Implement a Linux System Call

## setup

This project contains a *Vagrantfile* that runs a Fedora 31 VM.

```bash
./setup.sh
```

## Steps

### Get the linux kernel source

  ```bash
  # Since Fedora 31 uses kernel v5.3.7, let's get the closest patch version 5.3.14
  [vagrant@localhost ~]$ curl -OJ https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.3.14.tar.xz
  [vagrant@localhost ~]$ unxz -v linux-5.3.14.tar.xz

  # Optional - verify the distribution
  [vagrant@localhost ~]$ curl -OJ https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.3.14.tar.sign
  [vagrant@localhost ~]$ gpg --verify linux-5.3.14.tar.sign

  # Fetch the keys if they are missing. <KEY> is the key mentioned in above step.
  [vagrant@localhost ~]$ gpg --recv-keys <KEY>

  [vagrant@localhost ~]$ tar xvf linux-5.3.14.tar
  [vagrant@localhost ~]$ cd linux-5.3.14
  ```

### Make the code changes

> The code change steps assume the current directory is `~/linux-5.3.14`.

* The implementation of the system call is in [sys_greet_o.c](./code/greet_o/sys_greet_o.c) file and the build configuration is in [Makefile](./code/greet_o/Makefile) in `code/greet_o` directory. This directory will be available as `/vagrant/code/greet_o` in the VM. Copy it under the linux kernel source root directory.

  ```bash
  [vagrant@localhost ~]$ cp -r /vagrant/code/greet_o .
  ```

* Add the system call function header at the end of `include/linux/syscalls.h` file before the ending `#endif`.

  ```c
  asmlinkage long sys_greet_o(const char __user *str,
                              const size_t str_len,
                              char __user *buf,
                              size_t buf_len);
  ```

* Add the system call number to the end of the `arch/x86/entry/syscalls/syscall_64.tbl` file. I'm using **999** as system call number.

  ```c
  999     64      greet_o                 __x64_sys_greet_o
  ```

* Update the following line in the `Makefile` in the kernel source root directory to add `greet_o` directory:

  ```bash
  core-y += kernel/ certs/ mm/ fs/ greet_o/ ipc/ security/ crypto/ block/
  ```

### Build the kernel

  > The kernel build steps assume the current directory is `~/linux-5.3.14`.

  ```bash
  # copy the configuration from existing kernel
  [vagrant@localhost linux-5.3.14]$ cp /boot/config-$(uname -r) .config

  [vagrant@localhost linux-5.3.14]$ make menuconfig
  # The above command will pop open the config dialog. Just save and exit.

  # Optionally update CONFIG_LOCALVERSION in .config file to to build kernel with some suffix.
  # Here is the example that pleases my vanity :) - CONFIG_LOCALVERSION="-omkar"

  # $(nproc) is the number of cpus available. For this VM, it's 4.

  [vagrant@localhost linux-5.3.14]$ sudo make -j $(nproc)
  [vagrant@localhost linux-5.3.14]$ sudo make modules_install -j $(nproc)
  [vagrant@localhost linux-5.3.14]$ sudo make install -j $(nproc)
  ```

Once the build is successful, restart the virtual machine.

  ```bash
  # Exit the VM
  [vagrant@localhost linux-5.3.14]$ exit

  # Restart the VM and connect to it
  vagrant reload && vagrant ssh
  ```

### Verify

* Check the kernel version.

  ```bash
  [vagrant@localhost ~]$ uname -mrs
  Linux 5.3.14-omkar x86_64

  # See that's my own linux ;)!
  ```

* Compile and run the `/vagrant/code/test_driver.c` file to see the custom syscall in action.

  ```bash
  [vagrant@localhost ~]$ cd /vagrant/code
  [vagrant@localhost code]$ gcc test_driver.c -o test_driver
  [vagrant@localhost code]$ ./test_driver
  greet_o system call message: Namaste Omkar!

  # Yay, it's working!
  ```

* Check the kernel logs.

  ```bash
  [vagrant@localhost code]$ dmesg
  [  151.368231] greet_o system call invoked.
  [  151.368428] greet_o system call message: Namaste Omkar!
  ```

* Finally let's actually trace the system calls usage using `strace` tool.

  ```bash
  [vagrant@localhost code]$ strace ./test_driver
  ...
  ...
  ...
  syscall_0x3e7(0x7ffc0d747b80, 0x6, 0x7ffc0d747b40, 0x40, 0x7feb91c84f30, 0x7ffc0d747c88) = 0
  ...
  exit_group(0)                           = ?
  +++ exited with 0 +++
  ```

  **syscall_0x3e7** is our own system call, **0x3e7** being the hex value of **999**. The trace shows that the call successfully completed with return value of 0.
