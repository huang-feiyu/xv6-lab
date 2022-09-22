# lock

> In [lock](https://pdos.csail.mit.edu/6.S081/2020/labs/lock.html) lab, you'll
> gain experience in re-designing code to increase parallelism

## Memory Allocator

In order to reduce contention, re-design both data structures and locking
strategies for the xv6 memory allocator.

The basic idea is to maintain a free list per CPU, each list with its own lock.
Allocations and frees on different CPUs can run in parallel, because each CPU
will operate on a different list.
