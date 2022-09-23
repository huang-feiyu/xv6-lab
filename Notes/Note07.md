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

It is complex, and even more complex in real world OS.

