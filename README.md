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
-  // allocate phisical pages one by one
+  // allocate phisical pages one by one, aka. spread by time
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

+ addr = PGROUNDUP(addr);
+ addr = PGROUNDDOWN(addr);
```

### Lazytests and Usertests

Add some error handling code according to the hints.

##### DEBUG

<b>*</b> only sbrkarg FAILED => bug02

`sbrk()` in sbrkarg won't stop. It seems like my allocation doesn't work.

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
+   walk(pagetable, va, 0);
+ }
  if((*pte & PTE_U) == 0)
    return 0;
  pa = PTE2PA(*pte);
  return pa;
}
```

TODO: Need an explaination.
