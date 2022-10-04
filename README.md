# lazy

> lab 5: Report

## Analysis on content

In Unix, application ask the kernel for heap memory using `sbrk()` syscall. But
in general, user program **always** acquire \[much] more memory, which will take
a long time to allocate so many pages. In this lab, we are going to do some
optimization on it: lazy allocation.

This lab is divided into 3 parts, but they are a whole. Task#1 just tells us
what `sbrk()` is. Task#2 is the main part of lazy allocation. Task#3 requires
us to do some exception handling.

## Lazy allocation: Design and Analysis

We will use page table and page fault to implement lazy allocation. To allocate
physical memory lazily, when user ask `sbrk()` to change its heap memory,
**only** increase the heap size without allocating physical pages. When user
encounters a page fault (load page fault or store page fault), handle it by
allocate one physical page once. Do same thing when do software translation.

1. User ask `sbrk()` to change its heap memory<br/>
   (1) n < 0, decrease heap memory, call `uvmdealloc()` as before<br/>
   (2) n > 0, increase heap memory size without doing any process on physical
       page
2. User wanna read/write to the "unallocated" addr, aka. the address we handle
   in `sbrk()`. It will trigger a load/store page fault.<br/>
   The rest job is obivious: allocate physical page for the "unallocated" addr
3. `pgalloc()`: allocate physical address for VA<br/>
   (1) check if is valid<br/>
   (2) call `kalloc()` to allocate physical page on VA<br/>
   (3) call `mappages()` to map visual address to physical address on given
       page table
4. software translation: xv6 use `copyout()`/`copyin()` to move data back and
forth between kernel space and user space. The two functions use `walkaddr()` to
do the main work: find physical page in user pgtbl by given VA. If the address
is invalid, it will cause load/store page fault normally, we must use the same
handler to handle with *page fault*.<br/>
   In my solution, I just add a condition stmt to check if the address is valid.
   If is, then do nothing; call `pgalloc()`, otherwise.

---

> The following is notes while doing lab.

# lazy

> [lazy](https://pdos.csail.mit.edu/6.S081/2020/labs/lazy.html) lab forces you
> understand *lazy allocation* deeply.

### Eliminate allocation from `sbrk()`

Just follow the guide. (*Lazy allocation* counts only when there is an *increase*)

---

`char *sbrk(int n)`: Grow process’s memory by n bytes. Returns start of new memory.

### Lazy allocation

* scause:
  * 12: *instruction* page fault caused by an instruction fetch
  * 13: *load* page fault caused by a `read()`
  * 15: *store/AMO* page fault cause by a `write()`
* stval: the address that couldn't be translated

When a page fault ocurs, it will raise trap to allocate physical memory for the
virtual address stored in `stval`.

##### DEBUG

<b>*</b> map too soon => bug00

```diff
+  // allocate phisical pages one by one, aka. spread by time
-  // allocate phisical pages one by one
-  for (a = addr; a < sz; a += PGSIZE) {
     mem = kalloc();
     if (mem == 0) return -2;
     memset(mem, 0, PGSIZE); // fill with junk

-    if (mappages(myproc()->pagetable, a, PGSIZE, (uint64)mem, PTE_W|PTE_X|PTE_R|PTE_U) != 0) {
+    if (mappages(myproc()->pagetable, addr, PGSIZE, (uint64)mem, PTE_W|PTE_X|PTE_R|PTE_U) != 0) {
       kfree(mem);
       return -3;
     }
-  }
```

<b>*</b> "remap" raised by `mappages()` => bug01

Output: Page table skip pte[4].
```diff
===page fault: process success===

page table 0x0000000087f75000
..0: pte 0x0000000021fdc801 pa 0x0000000087f72000
.. ..0: pte 0x0000000021fd9401 pa 0x0000000087f65000
.. .. ..0: pte 0x0000000021fdc05f pa 0x0000000087f70000
.. .. ..1: pte 0x0000000021fd98df pa 0x0000000087f66000
.. .. ..2: pte 0x0000000021fdc40f pa 0x0000000087f71000
.. .. ..3: pte 0x0000000021fd68df pa 0x0000000087f5a000
+. .. ..5: pte 0x0000000021fd641f pa 0x0000000087f59000
..255: pte 0x0000000021fdd001 pa 0x0000000087f74000
.. ..511: pte 0x0000000021fdcc01 pa 0x0000000087f73000
.. .. ..510: pte 0x0000000021fd90c7 pa 0x0000000087f64000
.. .. ..511: pte 0x0000000020001c4b pa 0x0000000080007000
```

After adding pte[5], it replayed the page fault process.

```diff
int
pgalloc()
{
  char *mem;
  uint64 addr = r_stval();  // VA caused exception
  uint64 sz = myproc()->sz; // sbrk "has" allocated memory addr

  if (sz <= addr) return -1;

- addr = PGROUNDUP(addr);
+ addr = PGROUNDDOWN(addr);
```

### Lazytests and Usertests

Add some error handling code according to the hints.

##### DEBUG

<b>*</b> only sbrkarg FAILED => bug02

<i>`sbrk()` in sbrkarg won't stop. It seems like my allocation doesn't work.</i>

```diff
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  if(va >= MAXVA)
    return 0;

  pte = walk(pagetable, va, 0);
- if(pte == 0)
-   return 0;
- if((*pte & PTE_V) == 0)
-   return 0;
+ // idea from others
+ if(pte == 0 || (*pte & PTE_V) == 0){
+   if(pgalloc(va)){
+     return 0;
+   }
+   pte = walk(pagetable, va, 0);
+ }
  if((*pte & PTE_U) == 0)
    return 0;
  pa = PTE2PA(*pte);
  return pa;
}
```

Explaination:

> Handle the case in which a process passes a valid address from sbrk() to a
> system call such as read or write, but the memory for that address has not yet
> been allocated. (Hint from pdos)

When a `write()` or `read` syscall attempts to access unallocated address,
syscall does **not** use page table hardware to access -> no page fault.
We need a process while syscall accessing the unallocated address.

---

```
Time:   6 Hours
Score:  119/119
Author: Huang
Date:   2022-09-10
```
