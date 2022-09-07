# syscall

> [syscall](https://pdos.csail.mit.edu/6.S081/2021/labs/syscall.html) lab Help
> you understand how they work and will expose you to some of the internals of
> the xv6 kernel.

## System call tracing

Add system call tracing feature to print sycall info, we need to add a syscall
`trace`. Follow the guide and do it.

```
Usage: trace mask cmd [args]

- mask: 1 << SYS_call1 | 1 << SYS_call2
```

Just follow the guide, it is fairly easy to implement.
