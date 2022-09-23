# lock

> In [lock](https://pdos.csail.mit.edu/6.S081/2020/labs/lock.html) lab, you'll
> gain experience in re-designing code to increase parallelism

## Memory Allocator

In order to reduce contention, re-design both data structures and locking
strategies for the xv6 memory allocator.

The basic idea is to maintain a free list per CPU, each list with its own lock.
Allocations and frees on different CPUs can run in parallel, because each CPU
will operate on a different list.

### Debug

<b>*</b> panic: init exiting => bug01

Need to handle: when the cpu's freelist is empty, we need to steal from another
cpu. => Add some process in `kalloc()`.

<b>*</b> no kmem lock print => bug02

```diff
struct {
  struct spinlock lock;
  struct run *freelist;
+ char lockname[16];
} kmem[NCPU];

void
kinit()
{
- char lockname[16];
  for(int i = 0; i < NCPU; i++){
-   memset(lockname, 0, 16); // clear buffer
-   snprintf(lockname, 16, "kmem[%d]", i);
-   initlock(&kmem[i].lock, lockname);
+   snprintf(kmem[i].lockname, 16, "kmem[%d]", i);
+   initlock(&kmem[i].lock, kmem[i].lockname);
  }
  freerange(end, (void*)PHYSTOP);
}
```

<b>*</b> kernel panic: load page fault => bug03

I think I have done nothing, but it works well... Weird concurrent programming.

```diff
-  if(kmem[id].freelist){
-    acquire(&kmem[id].lock);
-    r = kmem[id].freelist;
+  acquire(&kmem[id].lock);
+  r = kmem[id].freelist;
+  if(r){
     kmem[id].freelist = r->next;
-    release(&kmem[id].lock);
   } else {
     int aid; // another cpu id
     for(int i = 1; i < NCPU; i++){
       aid = (id + i) % NCPU;
-      if(kmem[aid].freelist) break;
+      acquire(&kmem[aid].lock);
+      if(kmem[aid].freelist){
+        r = kmem[aid].freelist;
+        kmem[aid].freelist = r->next;
+        release(&kmem[aid].lock);
+        break;
+      }
+      release(&kmem[aid].lock);
     }
-    acquire(&kmem[aid].lock);
-    r = kmem[aid].freelist;
-    kmem[aid].freelist = r->next;
-    release(&kmem[aid].lock);
   }
+  release(&kmem[id].lock);
```

## Buffer Cache

What we need to do is: process the cache buffers of different hash buckets
concurrently.

The main challenge is to figure out the relationship between buf and bucket.
In my implementation, it is like this: (<s>modifed sth, it is a cycle linked list now</s>)

<img src="https://user-images.githubusercontent.com/70138429/191880045-5c22aa54-3476-423d-b97d-ca584ac32c06.png" width="400px"></img>

### Debug

<b>*</b> Stuck at bget first for-loop => bug04

Use 0 as terminal, donot use cycle linked list.

<b>*</b> panic: no buffers => bug05

It seems like every buffer has been used<s>, not a bug?</s>
