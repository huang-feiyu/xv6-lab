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
