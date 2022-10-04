# cow

> lab 6: Report

## Analysis on content

In Unix, we are likely to use the scheme fork-exec. As I mentioned in lazy lab,
doing stuff on physical page is expensive. Hence, we need to do some
optimization while using `fork()`. That's why Copy-on-Write was invented.

Before implement COW, when we call `fork()`, xv6 will entirely copy all pages
from parent to child. But in COW, when we call `fork()`, xv6 will **not** copy
anything immediately but mark every PTE to **un-writable**. Only if a process
wants to write to this specific un-writable page (COW page), we copy the page
content to the new allocated page. This lab is very similar to lazy lab, we also
need to handle expections and software translation.

## Copy-on-Write: Design and Analysis

When a user process calls `fork()`, do not copy physical pages immediately, but
mark every PTEs COW in old page table and **remap** the physical page to new
**page table**. If user program want to **write** to COW page, will encounter
a store page fault. We need to handle with COW page fault. In my case, I use
`cowcopy()` to copy physical page and add mapping to the process's page table.
To avoid re-free a same physical page, we need to maintain a refcnt for each
physical page. Only if the page's refcnt is zero, we will really free it.

1. `uvmcopy`: map parent's physical page to child, clear PTE_W and set PTE_C
   for both child and parent
2. `usertrap`: handle store page fault on COW page, copy the old to the new when
   refcnt > 1, just use the page if refcnt = 1<br/>
   For new physical page, install it to new page table, clear COW flag and set
   it writable<br/>
   For old physical page, call `kfree()` to decrement refcnt of it
3. Maintain refcnt<br/>
   (1) Set refcnt when `kalloc` allocates it<br/>
   (2) Increment when `fork` causes child share it<br/>
   (3) Decrement when `usertrap` handler drop it<br/>
   (4) Decrement when `kfree` drop it, only if refcnt = 0, `kfree` place it back
       to freelist
4. `copyout()`: use the same scheme as trap hdlr when it encounters a COW page<br/>
   One thing to note here: COW page will only trigger store page table, aka.
   `copyout()` of software translation. So, do not handle it in `walkaddr`.

---

> The following is notes while doing lab.

# cow

> [cow](https://pdos.csail.mit.edu/6.S081/2020/labs/cow.html) lab will add a
> feature to xv6: Copy-on-Write fork().

## Prepare

The goal of copy-on-write (COW) fork() is to defer allocating and copying
physical memory pages for the child until the copies are actually needed, if
ever.

## COW

There two main jobs we need to do:
1. Handle page fault on a cow page
2. Maintain reference count to each page

* Prototype
For COW mapping, use RSW in PTE to record.
For reference count, maintain a fixed-size array of indexes to page.

### DEBUG

<b>*</b> User program cannot stop => bug01

```diff
  refcnt[PG_INDEX((uint64)pa)]--;
- if(refcnt[PG_INDEX((uint64)pa)] == 0)
+ if(refcnt[PG_INDEX((uint64)pa)] != 0)
    return; // if someone is still using it, do nothing
```

<b>*</b> Kernel page fault => bug02

```diff
  int
  cowcopy(uint64 va)
  {
+   va = PGROUNDDOWN(va);
    struct proc *p = myproc();
```

<b>*</b> filetest read error => bug03

It seems like that my `copyout` simulating COW faults failed. Okay, it is.
I use the same approach as lazy lab, aka. <s>handle page fault in `walkaddr`</s>.
But I do not need to handle with load page fault, but **only store page fault**.

```diff
  copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len){
    uint64 n, va0, pa0;

    while(len > 0){
      va0 = PGROUNDDOWN(dstva);
+     if(cowcopy(pagetable, va0))
+       return -1;
      pa0 = walkaddr(pagetable, va0);
```

<b>*</b> `copyout` failed immediately: panic: kerneltrap. => bug04

`copyout` pass invalid pointers, we need to add some if stmts to deal with.

```diff
  int
  cowcopy(pagetable_t pagetable, uint64 va)
  {
    va = PGROUNDDOWN(va);
+   if(va >= MAXVA) return -1;
    pte_t *pte = walk(pagetable, va, 0);
+   if(pte == 0) return -1;
```

<b>*</b> FAILED -- lost some free pages 32443 (out of 32444) => bug05

According to others, it seems like concurrency issue. Some processes call
`kfree` the same time.

```diff
void
kfree(void *pa)
{
+ acquire(&refcnt_lock);
  if(refcnt[PG_INDEX((uint64)pa)] == 0)
    goto free; // for freerange init

  refcnt[PG_INDEX((uint64)pa)]--;
  if(refcnt[PG_INDEX((uint64)pa)] != 0){
+   release(&refcnt_lock);
    return; // if someone is still using it, do nothing
  }

 free:
+ release(&refcnt_lock);
```

---

```
Time:   7 Hours
Score:  110/110
Author: Huang
Date:   2022-10-03
```
