# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "fedora/31-cloud-base"
  if Vagrant.has_plugin?("vagrant-vbguest")
    config.vbguest.auto_update = false
  end
  config.vm.provider "virtualbox" do |v|
    v.name = "om_syscall"
    v.cpus = 4
    # Memory needed to build the kernel
    # Otherwise you'll get a cryptic error -
    # scripts/link-vmlinux.sh: line 45: 72516 Killed  ${LD} ${KBUILD_LDFLAGS} -r -o ${1} ${objects} make: *** [Makefile:1054: vmlinux] Error 137
    # and the real reason can b found by looking at system log by using command: dmesg
    # [  114.359002] Out of memory: Killed process 1304 (semodule) total-vm:213140kB, anon-rss:200548kB, file-rss:4kB, shmem-rss:0kB
    v.memory = 8192
  end

  config.vm.provision "shell", privileged: false, inline: <<-SHELL
    echo 'Running dnf upgrade...'
    sudo dnf upgrade -y > /dev/null
    echo 'Installing required packages...'
    sudo dnf group install "Development Tools" -y > /dev/null
    sudo dnf install vim-enhanced ncurses-devel bison flex elfutils-libelf-devel openssl-devel bc strace -y > /dev/null
  SHELL
end
