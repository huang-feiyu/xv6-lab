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

/*
 * sys_mmap - Create a new mapping in the virtual address space of the calling process
 *          - Huang (c) 2022-10-05
 */
uint64
sys_mmap(void)
{
  int len, prot, flags, fd, offset;
  uint64 addr;
  struct file *file;

  struct proc *p = myproc();

  if(argaddr(0, &addr) < 0 || argint(1, &len) < 0 || argint(2, &prot) < 0 ||
    argint(3, &flags) < 0 || argfd(4, &fd, &file) < 0 || argint(5, &offset) < 0)
    return -1;

  // xv6 assumption
  if(addr != 0 || offset != 0)
    return -1;

  // validate arguments
  if(len <= 0 || len % PGSIZE != 0 || file == 0)
    return -1;

  // check if there is enough space
  if(p->sz >= p->VMA_START - len)
    return -1;

  for(int i = 0; i < NVMA; i++){
    if(p->vma[i].len == 0){
      p->vma[i].len = len;
      p->vma[i].start = p->VMA_START - len;
      p->vma[i].end = p->VMA_START;
      p->vma[i].prot = prot;
      p->vma[i].flags = flags;
      p->vma[i].offset = offset;
      p->vma[i].file = file;

      p->VMA_START -= len; // grow from top to bottom
      filedup(file);       // increment refcnt

      // must be page-aligned
      if(p->vma[i].start % PGSIZE != 0 || p->vma[i].end % PGSIZE != 0)
        panic("sys_mmap: vma must be page-aligned");

      return p->vma[i].start;
    }
  }

  panic("sys_mmap: no more vma");
}

/*
 * sys_munmap - Delete the mappings for the specified address range
 *            - Huang (c) 2022-10-05
 */
uint64
sys_munmap(void)
{
  return -1;
}
