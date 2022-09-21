#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

/**
 * sys_sigalarm - Install an alarm signal handler to proc
 *              - Huang (c) 2022-09-21
 */
uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 hdlr;

  if(argint(0, &ticks) < 0 || argaddr(1, &hdlr) < 0)
    return -1;
  if(ticks < 0) return -1;

  struct proc *p = myproc();
  p->alarmticks = ticks;
  p->alarmhdlr = hdlr;
  p->duration = 0;

  return 0;
}

/**
 * sys_sigreturn - Restore process context when handler returns
 *               - Huang (c) 2022-09-21
 */
uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  if(p->alarmticks == 0)
    return -1;

  // restore trapframe
  *p->trapframe = *p->alarmtf;

  return 0;
}
