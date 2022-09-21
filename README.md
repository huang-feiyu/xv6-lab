# traps

> [traps](https://pdos.csail.mit.edu/6.S081/2020/labs/traps.html) lab explores
> how system calls are implemented using traps.

## Backtrace

Just follow the guide, print ret addr first, then update to prev frame pointer.

## Alarm

Add a syscall in xv6 to periodically alert a process as it uses CPU time.

For back to previous state, we should store the tramframe when call handler.
