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

### Debug

It seems like HITsz updated the test file, need to add syscall's first argument
to the output.

```diff
void
syscall(void)
{
- int num;
+ int num, arg;
  struct proc *p = myproc();

  num = p->trapframe->a7;
+ arg = p->trapframe->a0;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->trapframe->a0 = syscalls[num]();

    // only trace sets the mask
    if (p->mask > 0 && p->mask & (1 << num))
-     printf("%d: syscall %s -> %d\n",
-             p->pid, sysname[num], (int)p->trapframe->a0);
+     printf("%d: sys_%s(%d) -> %d\n",
+             p->pid, sysname[num], arg, (int)p->trapframe->a0);

  } else {
    printf("%d %s: unknown sys call %d\n",
```
