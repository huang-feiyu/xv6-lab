# mmap

> lab 10: Report

## Analysis on content

In this lab, we are going to implement an important [toy] feature called `mmap`.
Memory-mapped (`mmap`) file I/O is an OS-provided feature that maps the contents
of a file on secondary storage into a program’s address space.

In traditional opinion, `mmap` seems efficient and easy to handle. It reduces
data copy, after all. On the other hand, the OS created illusion makes user
pointer really read/write on the disk. So, it is easy to deal with.

For further information, just read [mmap is 💩](https://telegra.ph/mmap-internals--why-it-sucks-in-DBMS-10-08).

## Memory Map: Design and Analysis

First, we need to know how `mmap` works in the OS:

![img](https://user-images.githubusercontent.com/70138429/194573687-33250251-c42e-4b7e-9db8-3639b724ca42.png)

1. A user program calls `mmap` and gets a pointer to the VMA (**V**irtual
   **M**emory **A**rea)<br/>
   For the user program, the pointer is in its virtual memory and is the same as
   any other pointer.
2. In this segment, `mmap` only finds big enough virtual memory space and
   declares that VMA belongs to it.<br/>
   `mmap` is a system call, so the work is done in kernel space. OS initializes
   VMA struct for user process instead of allocating physical memory, or copying
   any data.
3. The user program attempts to write data to the file using the pointer.
4. CPU attempts to retrieve the page and write to physical memory.<br/>
   CPU tells MMU (**M**emory **M**anagement **U**nit) to find the *physical
   address* in the page table using a *virtual address*. But there is no such
   *physical address*, so it causes a *page fault* trap for OS.
5. OS knows it is a *page fault* and calls the *page fault handler* to deal with
   it.<br/>The handler copies the disk block to memory.
6. The handler also adds a mapping to the user page table.
7. CPU adds an entry in its TLB (**T**ranslation **L**ookaside **B**uffer) to
   accelerate future accesses.
8. Redo 4 to write the data to memory
9. The user program calls `munmap` to write the dirty page for persistence or
   just throw everything.

---

Then, follow the MIT guide to implement it.

```
# user address space
+------------+            <- MAXVA
| trampoline |
+------------+
| trapframe  |
+------------+            <- VMAs, always grow up to bottom (NO shrink)
|            |               (In this lab, a better/complex approach is:
|    HEAP    |                             sort the VMA by addr asc,
|            |                             choose the best fit area)
+------------+
|   stack    |
+------------+
|    ...     |
+------------+            <- 0
```

VMA records meta-data of the memory mapped region, such as permissions, length...

1. Define VMA to keep track of what `mmap` has mapped for each process.<br/>
   Add fixed-size VMAs to `struct proc`, init members in `allocproc`.
2. Implement `mmap` (Lazy allocation)<br/>
   (1) Find free space to map, I use the space described above<br/>
   (2) Allocate VMA and fill up the content<br/>
   (3) Increment file refcnt
3. Handle page fault in a mmap-ed region:<br/>
   (0) Validate arguments<br/>
   (1) Allocate physical memory using `kalloc`<br/>
   (2) Read 4KB data from file to the address using `iread`<br/>
   (3) Add mapping to user page table
4. Implement `munmap`<br/>
   (0) Validate arguments<br/>
   (1) Find the VMA for the address range<br/>
   (2) If MAP_SHARED, write the page back<br/>
   (3) Unmap specified pages using `uvmunmap`.
       If remove all pages, decrement refcnt of file
5. `exit`: travel through all VMAs, if len != 0, then unmap all pages.
6. `fork`: copy VMAs to child.<br/>
   Only one thing to mention here: do not forget to increment file refcnt.

---

> The following is note while doing this lab.

# mmap

> In this [lab](https://pdos.csail.mit.edu/6.S081/2020/labs/mmap.html), you will
> implement `mmap` & `munmap` focusing on memory-mapped files.

Before this lab, take MIT 6.S081 [Lecture 17](https://mit-public-courses-cn-translatio.gitbook.io/mit6-s081/lec17-virtual-memory-for-applications-frans).

## Prepare

```c
/**
 * Create a new mapping in the virtual address space of the calling process.
 *
 * @param addr   the starting address for the new mapping
 * @param length specifies the length of the mapping (must be greater than 0)
 * @param fd     file descriptor; close when return
 * @param offset mapping contents start at file offset (multiple page size)
 *
 * @return the address of the new mapping is returned as the result of the call
 */
void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);

/**
 * Delete the mappings for the specified address range
 */
int munmap(void *addr, size_t length);
```

In this lab, we only implement subset relevant to memory-mapping a file.
* `mmap`
  * `addr`: always be 0 => kernel should choose the VA to map the file
  * `length`: the number of bytes to map
  * `prot`: indicates whether the memory should be mapped readable, writeable,
    and/or executable; [PROT_READ PROT_WRITE PROT_READ&PROT_WRITE]
  * `flags`: *Either Or*<br/>
    (1) MAP_SHARED: modifications should be written back to the file<br/>
    (2) MAP_PRIVATE: do not write back
  * `fd`: the open file descriptor of the file
* `munmap`: remove mmap mappings in the address range
  * If mapped MAP_SHARED, write back to file first
  * You can assume that `munmap` will not punch a hole in the middle of a region

## Memory Map

1. Define VMA to keep track of what `mmap` has mapped for each process.<br/>
   Add fixed-size VMAs to `struct proc`, init members in `allocproc`.
2. Implement `mmap` (Lazy allocation)<br/>
   (1) Find free space to map, I use the approach described above<br/>
   (2) Allocate VMA
3. Handle page fault in a mmap-ed region:<br/>
   (1) Read 4KB data from file to memory<br/>
   (2) Add mapping to user page table
4. Implement `munmap`<br/>
   (1) Find the VMA for the address range<br/>
   (2) If MAP_SHARED, write the page back<br/>
   (3) Unmap specified pages. If remove all pages, decrement refcnt of file
5. `exit` to unmap the process's mapped regions as if munmap had been called
6. `fork` ensure that the child has the same mapped regions as the parent

### DEBUG

<b>*</b> cannot do mapping twice => bug01

```diff
mpgalloc(va){

- if(readi(p->vma[i].file->ip, 0, (uint64)mem, offset, PGSIZE) != PGSIZE)
+ if(readi(p->vma[i].file->ip, 0, (uint64)mem, offset, PGSIZE) == -1)
    return -6;
```

<b>*</b> permission issue => bug02

```diff
uint64
sys_mmap(void)
{

+ if(prot & PROT_READ)
+   if(!file->readable)
+     return -1;

+ if(prot & PROT_WRITE)
+   if(!file->writable && !(flags & MAP_PRIVATE))
+     return -1;
```

<b>*</b> uvmunmap: not mapped => bug03

```diff
uint64
sys_mmap(void)
{
+ // unmap specified pages (has mmapped)
+ if(walkaddr(p->pagetable, addr) != 0)
+   uvmunmap(p->pagetable, addr, len / PGSIZE, 1);
- uvmunmap(p->pagetable, addr, len / PGSIZE, 1);
```

---

```
Time:   11 Hours
Score:  140/140
Author: Huang
Date:   2022-10-06
```
