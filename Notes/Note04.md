# Chapter 4

> Traps and system calls

[TOC]

### Intro

trap types:

1. syscall via `ecall` (traps from user space)
2. exception: /0 error or use invalid VA (traps from kernel space)
3. interrupt: disk hardware finishes reading (**timer interrupts**)

trap handling:

1. CPU takes hardware actions<br/>xv6-book page 44
2. assembly instructions prepares for kernel C code (**vector**)
3. a C function decides what to do next (**handler**)
4. system call or device-driver service routine

### Hardware

stvec: S-mode Trap-Vector Base Address (trap handler address)

sepc: S-mode Exception PC

scause: S-mode Trap cause

sscratch: S-mode scratch (very start of handler)

sstatus: S-mode status (SIE bit, whether device interrupts are enabled)

### User space

`uservec`(save registers in trapframe) -><br/>
`usertrap`(process trap) -><br/>
`usertrapret`(set up control registers) -><br/>
`userret`(restore page table & registers)

*trampoline* page is mapped in every process's page table at `TRAMPOLINE`

### Kernel space

`kernelvec` (save registers and switch to another thread) -><br/>
`kerneltrap` (process trap)

TODO: deeper understanding

### Page-fault exceptions

*Page Fault*:

1. load page faults (ld cannot translate its VA)
2. store page fault (sd cannot translate its VA)
3. instruction page fault (the address in PC doesn't translate)

---

To reduce *page fault* or use it as a mechanism.

* COW (copy on write) fork:
  * parent and child initially share all physical pages (read-only)
  * if write to a shared page, copy it individually
* *lazy allocation*:
  1. a program asks for more memory by calling `sbrk`, but kernel does not
     allocate physical memory & does not create PTEs for the new range of VA
  2. on a page fault on one of those new addresses, the kernel allocates a page
     of physical memory and maps it into the page table
* *demand paging*
* *paging to disk*

---

MIT 6.S081 Lecture 6:
* 程序运行trap是完成用户空间和内核空间的切换。每当:
    * 程序执行系统调用
    * 程序出现了类似page fault、运算时除以0的错误
    * 一个设备触发了中断使得当前程序运行需要响应内核设备驱动
* Supervisor Mode:
    * 可以读写控制寄存器了。比如说，当你在supervisor mode时，你可以:
      读写SATP寄存器，也就是page table的指针；STVEC，也就是处理trap的内核指令地址;
      SEPC，保存当发生trap时的程序计数器；SSCRATCH等等。
    * 可以使用PTE_U标志位为0的PTE。当PTE_U标志位为1的时候，表明用户代码可以使用这个页表;
      如果这个标志位为0，则只有supervisor mode可以使用这个页表。(Kernel不能使用用户页表PTE_U=1)
* call: write() -> ecall -> uservec - trampoline -> usertrap() -> syscall() -> sys_write()
* ret: sys_write() -> syscall() -> usertrap() - usertrapret() -> userset - trampoline -> write()
* csrrw将a0的数据保存在了sscratch中，同时又将sscratch内的数据保存在a0中。之后内核就可以任意的使用a0寄存器
* sret: return to user mode from kernel mode
    * 程序会切换回user mode
    * SEPC寄存器的数值会被拷贝到PC寄存器（程序计数器）
    * 重新打开中断

