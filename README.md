# thread

> lab 7: Report

## Analysis on content

This lab contains 3 tasks:
1. Implement switching between threads in a user-level threads package in xv6
2. Use multiple threads to speed up a program
3. Implmement a barrier

The 3 tasks do not lead us to the kernel of xv6, but only tells us how to use
multi-thread programming. So, it is easy.

## switching between user-level threads: Design and Analysis

To switch between user-level thread, we need to store the context of the current
thread. In kernel, there is `struct context` for this purpose. Just steal it
from *kernel/proc.h*. Context is not only registers, but also stack content. So,
we need to allocate a stack for each thread.

1. `thread_create()`: Init the stack pointer, frame pointer and entry function
   address, store them into `sturct context`
2. `thread_schedule()`: Switch old thread to new, call assembly code to do this
   (The code is almost the same as *kernel/swtch.S*)

## Using threads

Add a mutex lock to each bucket, for each element, find its bucket and lock it
until go through all critical section. Nothing special.

## Barrier

Wait for condition variable, broadcast after all threads reach. Use a global
variable to store number of threads, if number of waiting threads has reached
nthread, then broadcast, wait otherwise.

---

> The following is notes while doing lab.

# thread

> [thread](https://pdos.csail.mit.edu/6.S081/2020/labs/thread.html) lab will
> familiarize you with multithreading.

## Uthread: switching between threads

Very similar to [M2: libco](http://jyywiki.cn/OS/2022/labs/M2).

* when `thread_schedule()` runs a given thread for the first time, the thread
  executes the function passed to `thread_create()`, on its own stack
* `thread_switch` saves the registers of the thread being switched away from,
  restores the registers of the thread being switched to

My design policy: steal from kernel... =>
It is fairly easy to implement, because the skeleton has done almost everything.

## Using thread

Explore parallel programming with threads and locks using a hash table.

Add a lock to each bucket, I should do this lab before lock.

## Barrier

Wait for condition variable, broadcast after all threads reach.

---

```
Time:   2 Hours
Score:  60/60
Author: Huang
Date:   2022-09-26
```
