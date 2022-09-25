# Chapter 7

> Scheduling

* Switch from one process to another
  1. sleep & wakeup when device or pipe I/O
  2. scheduler switching
* Sleep and Wakeup (*sequence coordinateion*)
  * Sleep: allows a kernel thread to wait for a specific event
  * Wakeup: another thread call it to indicate that threads waiting for an event
    should resume
* *semaphore*: TODO: understand its implementation
* Implementation of Sleep & Wakeup
  * Sleep: mark current process as SLEEPING, call sched to release CPU
  * Wakeup: looks for a process sleeping on the given wait channel and marks it
    as RUNNABLE

It is complex, and in real world OS even more complex.

---

MIT Lecture 11 & 12

* 线程: 单个串行执行代码的单元，它只占用一个CPU并且以普通的方式一个接一个的执行指令;
  has PC, Regs, Stack
* 中断处理的基本流程(*pre-emptive scheduling*): 定时器中断将CPU控制权给到内核,
  内核再自愿的出让CPU
* *voluntary scheduling*

在XV6中，任何时候调用switch函数都会从一个线程切换到另一个线程，
通常是在用户进程的内核线程和调度器线程之间切换。在调用switch函数之前，
总是会先获取线程对应的用户进程的锁。所以过程是这样，一个进程先获取自己的锁，
然后调用switch函数切换到调度器线程，调度器线程再释放进程锁。

1. 一个进程出于某种原因想要进入休眠状态，比如说出让CPU或者等待数据，它会先获取自己的锁；
2. 之后进程将自己的状态从RUNNING设置为RUNNABLE；
3. 之后进程调用switch函数，其实是调用sched函数在sched函数中再调用的switch函数；
4. switch函数将当前的线程切换到调度器线程；
5. 调度器线程之前也调用了switch函数，现在恢复执行会从自己的switch函数返回；
6. 返回之后，调度器线程会释放刚刚出让了CPU的进程的锁

TODO: Understand the code.

