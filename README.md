# thread

> [thread](https://pdos.csail.mit.edu/6.S081/2020/labs/thread.html) lab will
> familiarize you with multithreading.

## Uthread: switching between threads

Very similar to [M2:libco](http://jyywiki.cn/OS/2022/labs/M2).

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
