# lock

> lab 8: Report

Memory Allocator: In this task, we will implement a multi-thread safe data
structure for xv6's memory allocator. Roughly, we will implement **a lock per
freelist on cpu** to reduce contention. The main idea is: ensure serilization
in critical section using lock.

1. Init every lock in `kinit`
2. When `kalloc` is called, following the algorithm below:<br/>
  (1) Get the cpu id via `cpuid()` <br/>
  (2) Acquire the freelist lock for this cpu/freelist<br/>
  (3) Check whether there is a free run. If is, remove it from freelist, goto
      (5); Otherwise, continue to (4)<br/>
  (4) Walk through every freelist: Acquire its lock and check whether there is a
      free run. If is, move the free run to current freelist, goto (5). After
      checking release the lock.
      (If no such one, just return nullptr, panic whatever)
  (5) Release the freelist lock for this cpu/freelist
3. For `kfree`: Acquire current lock, add the run to freelist, release the lock


Buffer cache: Because not all operations need a big lock, we are going to use
lock with different granularities to ensure correctness. It's what xv6 wants me
to do, but I just use locks in specific order to avoiding using big lock.

Data structures: one bcache contains a big lock and multiple hash buckets. A
hash bucket contains an individual lock and a buf list.

<img src="https://user-images.githubusercontent.com/70138429/191880045-5c22aa54-3476-423d-b97d-ca584ac32c06.png" width="400px"></img>

* `binit`: Do initial stuff
  1. Init big lock of bcache and locks for all hash buckets
  2. Init buf list for each bucket. At first, add all bufs to bucket[0] (Other
     can steal from bucket[0])
* `bget`: Find a free buf and return<br/>
  Lock policy: always hold bucket[i]'s lock, walk through others' lock
  **orderly** (i.e, i+1 -> i+2 -> i+3 ... mod NBUF)
  1. Acquire bucket[i]'s lock
  2. Walk through bucket[i]'s buf list to check if already cached. If is,
     release lock and return.
  3. Otherwise, we need to steal a buf from others using LRU policy
     (Give up finding one within itself)
  4. Acquire bucket[i+j]'s lock
  5. Walk through bucket[i+j]'s buf list to find a LRU buf. If no such one,
    release bucket[i+j]'s lock and goto 4 for another bucket[i+j+1].
  6. Otherwise, move the LRU buf from bucket[i+j] to bucket[i], release the two
    locks and return
* `breles`: Decrement refcnt of a buf and maintain timestamp here
  1. Acquire bucket[i]'s lock
  2. Decrement refcnt and update timestamp
  3. Release the lock
* `bpin`/`bunpin`: Change big lock to bucket[i]'s lock

---

> The following is note while doing this lab.

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

If I do not use LRU policy, it is ok. There must be sth wrong with my LRU.

```diff
  for(b = bcache.bucket[p].head.next, pb = &bcache.bucket[p].head;
      b != 0; b = b->next, pb = pb->next)
    if(b->refcnt == 0){
      if(bp == 0 || (bp != 0 && bp->ticks < b->ticks)){
        bp = b;
        pbp = pb;
+       break;
      }
    }
```

<b>*</b> `break` issue => bug06

It seems like a lock issue: <s>We must call `bupdate` with a lock. But it is hard
to do it.</s>

* Update `bupdate` to extern int ticks
* Maintain ticks in `breles`

<s>Cannot fix this for now.</s>

```diff
- bcache.bucket[i].head.next = b;
+ bcache.bucket[i].head.next = bp;
```

<b>*</b> freeing free block => bug07

This is an issue about lock. Here is a hint from guide:
Searching in the hash table for a buffer and allocating an entry for that buffer
when the buffer is not found must be atomic.

<s>Okay, I failed to find out the bug. But changing the `NBUF` to
`MAXOPBLOCKS*12` works for me, it's an idea from web.</s>

First, add a big lock to ensure implementation works well in in-efficient way.

Then, re-design `bget` orderly:

Search in bucket to find whether cache hits
1. Cache Hits => return the buf
2. Cache Miss => <s>find unused buf in itself</s>
  1. <s>Find one => return the buf</s>
  2. Steal unused buf from others => get one OR panic

1 & 2.1 can be protected by **bucket** lock;
2.2 involves other **bucket** => need a big lock.

In the end, I choose to not find unused buf in bucket itself, so there is no
dead lock anymore. Because they all gain lock orderly, maintain atomicity by
holding bucket lock.

---

```
Time:   13 Hours
Score:  70/70
Author: Huang
Date:   2022-09-25
```
