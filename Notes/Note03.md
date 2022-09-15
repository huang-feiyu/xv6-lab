# Chapter 3

> Page Tables

[TOC]

### Hardware

> Paging Hardware: translates virtual addresses to physical addresses.

* $\text{Page table}: \text{VA} \to \text{PA}$
* Translate VA to PA: ([details](../xv6-book.pdf#page=33))
  1. Find [3-level] PTE & Offset from VA
  2. Find PPN from PTE
  3. Combine PPN & Offset to get PA
* TLB: caches page table entries (locates between CPU and CPU cache OR CPU cache and Mem)
* `satp` stores the root page-table address

### Kernel address space

* kernel can interact with the devices by reading/writing these special physical addresses
* kernel gets at RAM and memory-mapped device registers using “direct mapping”;<br/>undirect-mapped: trampoline page (mapped twice), kernel stack page (refers to page 35)

###  Process address space

[Figure 3.4: A process’s user address space, with its initial stack.](../xv6-book.pdf#page=38)

If the user stack overflows and the process tries to use an address below the stack, the hardware will generate a *page-fault* exception the guard page is inaccessible to a program running in user mode.

xv6 uses a process’s page table not just to tell the hardware how to map user virtual addresses, but also as the only record of which physical memory pages are allocated to that process.

---

MIT 6.S081 Lecture 4:
* page table保存在内存中，MMU只是会去查看page table。
  所以CPU中需要有一些寄存器用来存放表单在物理内存中的地址 => satp
* 如果你切换了page table，TLB中的缓存将不再有用，它们需要被清空，否则地址翻译可能会出错。
  在RISC-V中，清空TLB的指令是sfence_vma。
* 3级的page table由硬件实现的，所以3级 page table的查找都发生在硬件中。
  在XV6中，有一个函数也实现了page table的查找，因为时不时的XV6也需要完成硬件的工作,
  所以XV6有这个叫做walk的函数，它在软件中实现了MMU硬件相同的功能。
* 在一个RISC-V芯片中，有多个CPU核，MMU和TLB存在于每一个CPU核里面
* 由虚拟地址索引的cache位于MMU之前，由物理地址索引的cache位于MMU之后
* 内核会通过用户进程的page table，将用户的虚拟地址翻译得到物理地址，这样内核可以读写相应的物理内存地址

* PLIC是中断控制器(Platform-Level Interrupt Controller)
* CLINT(Core Local Interruptor)也是中断的一部分
* UART0(Universal Asynchronous Receiver/Transmitter)负责与Console和显示器交互
* VIRTIO disk，与磁盘进行交互

