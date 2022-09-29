# traps

> lab 4: Report

## Analysis on content

traps contains two tasks:
* Backtrace: implement a kernel function to print the backtrace of a queue of
  functions calls, which can somewhat indicate where the error is.
  * Backtrace just forces us to know **what the stack and frame pointer means**
* Alarm: add a syscall `sigalarm` to call periodically call a specific function
  * Alarm shows us **how xv6 handle with interrupts and traps**, we just need to
    do some stuff while handling clock interrupt
  * *clock interrupt*: the weapon that OS uses to control **every** process.
    OS uses it to deal with scheduling, unwanted program, etc.

## Backtrace: Design and Analysis

* *frame pointer*: the **base address** of the **current function**'s frame
* *stack pointer*: top of the stack/frame, where function can store sth
* *return address*: the next instruction in the calling method, where the
  function declaration code lands

After figure out the concepts, the design is obivious =>
1. Get current function's frame pointer and print return address
2. Walk up to previous stack frame, aka. update frame pointer to previous
3. Do (1) until the page-aligned address becomes invalid

```
  ...                                               HIGH addr
+-----+ <- current fp                                   |
| ret |                                                 |
+-----+                                                 |
|  fp | <- saved fp, aka. prev frame pointer            |
+-----+                                                 |
| ... |                                                 V
| ... | <- stack pointer                             LOW addr
```

## Alarm: Design and Analysis

To implement alerts a process periodcally, we need to **install hdlr** to the
process and maintain **duration** since last alert. That is what `sigalarm` does.
But hdlr doesn't know where to return to **prev instruction**, this acquires us
to store the **context** while calling hdlr and use a `sigreturn` in hdlr to
return back.

Add the members below into `struct proc`
```c
/* for sigalarm */
int duration;                // ticks since last call
int alarmticks;              // Alarm interval
uint64 alarmhdlr;            // Alarm handler address
/* for sigreturn */
struct trapframe *alarmtf;   // data page for alarm handler
```

Alarm design:
1. Install hdlr to current process: (`sigalarm`)<br/>
   Init related members of current
2. Every time interrupt, increments the process duration: (`usertrap`)<br/>
   (1) If reaches the threshold, store **current** stack frame, update next
       instruction to hdlr address. (the process will execute the hdlr when kernel
       yield to it)<br/>
   (2) Otherwise, do normal stuff
3. The process executed the hdlr and meets `sigreturn`:<br/>
   Restore process context when handler returns, and zero out the duration

---

> The following is the note **while doing** the lab.

# traps

> [traps](https://pdos.csail.mit.edu/6.S081/2020/labs/traps.html) lab explores
> how system calls are implemented using traps.

## Backtrace

Just follow the guide, print ret addr first, then update to prev frame pointer.

## Alarm

Add a syscall in xv6 to periodically alert a process as it uses CPU time.

For back to previous state, we should store the tramframe when call handler.

test2
```diff
 if(which_dev == 2){
   if(p->alarmticks != 0){
     p->duration++;
+    // equal to avoid retrant, duration clear after hldr return in sigreturn
     if(p->duration == p->alarmticks){
       // store current context
       *p->alarmtf = *p->trapframe;
       p->trapframe->epc = p->alarmhdlr;
-      p->duration = 0;
     }
   }
   yield();
 }
```

It is fairly easy to implement.

---

```
Time:   3 Hours
Score:  85/85
Author: Huang
Date:   2022-09-21
```
