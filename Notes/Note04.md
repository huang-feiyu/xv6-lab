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

