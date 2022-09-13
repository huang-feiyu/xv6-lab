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

<b>*</b> user program can only be executed once => bug03

Stuck at after `fork(void)`. I don't know why the order counts, but it works...
I think it is the kernel allocate the **same** process twice. The stack hasn't
been freed, which makes the 2nd `p->kstack` a wrong address.

```diff
procinit(void) {
  // code...
-      char *pa = kalloc();
-      if(pa == 0)
-        panic("kalloc");
-      p->kstack = (uint64)pa; // temporarily store in kstack
   }
   kvminithart();
}

---

  // Map a page for kernel stack
-  uint64 pa = p->kstack;
+  char *pa = kalloc();
+  if(pa == 0) panic("kalloc");
   uint64 va = KSTACK((int) (p - proc));
-  if(mappages(p->kpagetable, va, PGSIZE, pa, PTE_R | PTE_W) != 0)
+  if(mappages(p->kpagetable, va, PGSIZE, (uint64)pa, PTE_R | PTE_W) != 0)
     panic("allocproc: kernel stack mappages failed");
   p->kstack = va;
```

Another way to do this: maintain a `kstackpa` in `struct proc` to avoid freeing
kernel stack in `freeproc()`

## Simplify `copyin`/`copyinstr`

Add user page table mapping to kernel page table. Just copy the PTEs to kernel
page table like `uvmcopy()`.

* Update `userinit()` kernel page table
* Where kernel changes a process's user page table mapping, change the kernel too

### Debug

<b>*</b> panic: kerneltrap => bug04

scause 5: Load access fault

The bug disappeared, it is weird.

<b>*</b> bigdir failed => bug05

bigdir link(bd, x4Z) failed, but `make grade` got full credits.
