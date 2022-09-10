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
