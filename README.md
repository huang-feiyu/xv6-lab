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

