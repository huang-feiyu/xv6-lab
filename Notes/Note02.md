# Chapter 2

> Operating system Organization

[Kernel source file](../xv6-book.pdf#page=25)

* multiplexing:
    * *time sharing*
* isolation
    * forbid applications from directly accessing sensitive hardware resources,
      and instead to abstract the resources into services
    * Machine mode & Supervisor mode (*kernel space*, interrupt) & User mode (*user space*)<br/>
      Use `ecall` to raises the hardware privilege level and changes the program
      counter to a kernel-defined entry point
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

---

Take [NJU OS: Lecture 19](https://www.bilibili.com/video/BV1eA4y1Q76T/) for
better understand

Starting xv6:
1. Power on, computer initializes itself
2. Run a *boot loader* to load the xv6 kernel into memory
3. xv6 starting at `_entry`, set up stack & jump to `start`
4. `start` does configuration stuff in machine mode and init timer interrupt.
   Then, change to supervisor mode and jump to `main`
5. `main` initializes several devices and subsystem. It creates the first process
   by calling `userinit` -> `initcode` -> `ecall` -> `sys_exec` -> `/init`

---

MIT 6.S081 Lecture 2:
* 操作系统应该具有防御性(Defensive)
* 内核有时候也被称为可被信任的计算空间(Trusted Computing Base, TCB)
* QEMU main loop: Maintain states of registers
    * 读取4字节或者8字节的RISC-V指令。
    * 解析RISC-V指令，并找出对应的操作码(op code)。我们之前在看kernel.asm的时候,
      看过一些操作码的二进制版本。通过解析，或许可以知道这是一个ADD指令，或者是一个SUB指令。
    * 之后，在软件中执行相应的指令。
* userinit有点像是胶水代码/Glue code(胶水代码不实现具体的功能，只是为了适配不同的部分而存在)

