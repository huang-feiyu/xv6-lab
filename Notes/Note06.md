# Chapter 6

> Locking

If you program with locks, it is wise to **use a tool** that attempts to identify
race conditions.

* *concurrency*: multiple instruction streams are interleaved<br/>
  Due to multiprocessor parallelism, thread switching, interrupts
* *lock*: one of concurrency control techniques => mutual exclusion
* *race condition*: multiple accesses to a critical section
* *spin-lock*: If a spinlock is used by an interrupt handler, a CPU must never
  hold that lock with interrupts enabled
* *sleep-lock*: yileds the CPU while waiting to acquire and allows yields while
  the lock is held (leave the interrupts enabled); good for short critical
  sections and lengthy operations
* *deadlock*: need to acquire locks in the same order; danger: a contraint on
  how fine-grained one can make a locking scheme
  (more locks -> more opportunity for deadlock)
* *re-entrant lock*(*recursive lock*): 可重入锁
* lock-free programming is even harder than programming with locks

```c
/* spin lock */
// On RISC-V, sync_lock_test_and_set turns into an atomic swap:
//   a5 = 1
//   s1 = &lk->locked
//   amoswap.w.aq a5, a5, (s1)
while(__sync_lock_test_and_set(&lk->locked, 1) != 0)
  ;
```
