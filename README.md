# syscall

> [syscall](https://pdos.csail.mit.edu/6.S081/2021/labs/syscall.html) lab helps
> you understand how they work and will expose you to some of the internals of
> the xv6 kernel.

## Prepare

Very first syscall:
1. `main` inits devices and subsystems
2. `main` creates 1st process via `userinit`
3. `userinit` jumps to *initcode.S*
4. *initcode.S* executes `/init` via syscall `exec`

---

Inside user invoking syscall:
0. When *echo.c* call syscall `write`
1. User `main` jumps into *usys.S*
2. *usys.S* loads arg and executes `ecall`
3. `ecall` jumps to *trampoline.S*
4. *trampoline.S* saves context and continues to `syscall` in *syscall.c*
5. `syscall` executes the syscall

---

Add a syscall prototype:
1. declare in *user/user.h*
2. add stub to *user/usys.pl* to produce actual syscall stubs
3. add syscall number to *kernel/syscall.h*

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

## Sysinfo

Add a system call `sysinfo(struct sysinfo*)` to collect information about
running system.

* *kalloc.c*: Physical page allocator (xv6 Chapter 3.5)
  * a *free list* of physical memory pages
  * `kmem` is a global variable with a *lock* and a *free list*
* *proc.c*: Processes and scheduling
  * collect number of processes, read `procdump()`
  * collect number of free file descriptor

### Debug

<b>*</b> cannot end reading info => bug01

Output:
```
sys_sysinfo: start
sys_sysinfo: start reading info
```

GDB stuck at *kalloc.c* line 97.

```diff
- while((r = r->next)){
+ while(r){
    pagenum++;
+   r = r->next;
  }
```

<b>*</b> wrong nproc => bug02

```diff
  for(p = proc; p < &proc[NPROC]; p++){
    if(p->state == UNUSED) {
-     continue;
+     num++;
    }
-   num++;
  }
```

<b>*</b> wrong freefd => bug03

```diff
freefd(void)
{
  int num = 0;
- struct proc *p = proc;
+ struct proc *p = myproc();

- num = NELEM(p->ofile);
+ for(int i = 0; i < NOFILE; i++)
+   if(p->ofile[i] == 0)
+     num++;

  return num;
}
```

---

```
Time:   7 Hours
Score:  35/35
Author: Huang
Date:   2022-09-07
```
