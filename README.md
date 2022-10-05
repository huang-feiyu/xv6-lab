# mmap

> In this [lab](https://pdos.csail.mit.edu/6.S081/2020/labs/mmap.html), you will
> implement `mmap` & `munmap` focusing on memory-mapped files.

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

