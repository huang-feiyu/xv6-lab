# Chapter 2

> Operating system organization

[Kernel source file](../xv6-book.pdf#page=25)

* multiplexing:
    * *time sharing*
* isolation
    * forbid applications from directly accessing sensitive hardware resources,
      and instead to abstract the resources into services
    * Machine mode & Supervisor mode (*kernel space*, interrupt) & User mode (*user space*)<br/>
      Use `ecall` to switch to kernel space from user space, enter at a certain
      kernel entry point
    * *microkernel*: minimize the amount of operating system code that runs in
      kernel space, some of them runs in user space (such as file server)
    * process: an address space to give a process the illusion of its own memory,
      and, a thread to give the process the illusion of its own CPU
        * page table (by hardware): translate virtual address to physical address<br/>
          every process hold a page table
        * kernel stack & user stack
        * run state
    * security model
* iteraction
    * file system
    * file descriptor
    * pipe

