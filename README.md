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
