# pgtbl

> In this [lab](https://pdos.csail.mit.edu/6.S081/2020/labs/pgtbl.html) you will
> explore page tables and modify them to simplify the functions that copy data
> from user space to kernel space.

## Print a page table

Follow the guide.

## A kernel page table per process

Read [HITsz: pgtbl](https://hitsz-lab.gitee.io/os-labs-2021/lab4/part1/)
carefully, it helps a lot.

1. Maintain a sperated kernel page table per process
  * Create kpt via modified `kvminit()`, call it in `allocproc()`
  * Map kernel stack to process's kpt<br/>
    Allocate memory in `procinit()`, map in `allocproc()`
  * Free kpt in `freeproc()`
2. Change kernel page table while switching processes
  * Modify `scheduler()` to load process's kpt into `satp` register
  * `scheduler()` should use `kernel_pagetable` when no process is running

### Debug

<b>*</b> panic: remap => bug01

Remap kernel_pagetable init memory, add a `pvmmap()`.

<b>*</b> panic: kvmpa => bug02

Something goes wrong after `scheduler()` switched the context.

```diff
uint64
kvmpa(uint64 va)
{
  uint64 off = va % PGSIZE;
  pte_t *pte;
  uint64 pa;

+ pte = walk(myproc()->kpagetable, va, 0);
- pte = walk(kernel_pagetable, va, 0);
```
