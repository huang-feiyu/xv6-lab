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

