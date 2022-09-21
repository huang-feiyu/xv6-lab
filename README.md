# traps

> [traps](https://pdos.csail.mit.edu/6.S081/2020/labs/traps.html) lab explores
> how system calls are implemented using traps.

## Backtrace

The compiler puts in each stack frame a frame pointer that holds the address of
the caller's frame pointer. We should use these frame pointers to walk up the
stack and print the saved return address in each stack frame.

Note that the return address lives at a fixed offset (-8) from the frame pointer
of a stackframe, and that the saved frame pointer lives at fixed offset (-16)
from the frame pointer.
