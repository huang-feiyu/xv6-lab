// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

// I can just add the hashbucket to bcache directly,
// cause it is easier to understand
struct hashbucket {
  char lockname[16];
  struct spinlock lock;
  struct buf head; // each bucket head; dummy head, head.next matters
};

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  struct hashbucket bucket[NBUCKET]; // 13 buckets => 1 hash table
} bcache;

uint
hash(uint blockno)
{
  return blockno % NBUCKET;
}

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache"); // big lock

  // Init for every bucket
  for(int i = 0; i < NBUCKET; i++){
    snprintf(bcache.bucket[i].lockname, 16, "bcache[%d]", i);
    initlock(&bcache.bucket[i].lock, bcache.bucket[i].lockname);
    bcache.bucket[i].head.next = 0; // non-cycle linked list
  }
  // Init empty buffers, other buckets can steal from bucket[0]
  for(int i = 0; i < NBUF; i++){
    b = &bcache.buf[i];
    b->next = bcache.bucket[0].head.next;
    initsleeplock(&b->lock, "buffer");
    bcache.bucket[0].head.next = b;
    b->ticks = ticks;
  }
}


// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  uint i = hash(blockno); // needed bucket index
  uint p = i;             // finding in which bucket, pointer
#ifdef DEBUG
  uint cnt = 0;
#endif

  acquire(&bcache.bucket[i].lock);

  // Is the block already cached?
  for(b = bcache.bucket[i].head.next; b != 0; b = b->next){
#ifdef DEBUG
    cnt++;
    printf("bget: cnt[%d] %p %d\n", cnt, (uint64)b, b->refcnt);
#endif
    if(b->dev == dev && b->blockno == blockno){
      b->ticks = ticks; // for LRU
      b->refcnt++;
      release(&bcache.bucket[i].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  release(&bcache.bucket[i].lock);

  while(1){
    // apporximately LRU policy: (Bad design?)
    //  find the LRU buf in one bucket a time via timestamp
    //  * if there is, end
    //  * if there is none, change to another bucket
    acquire(&bcache.bucket[p].lock);
    struct buf *bp = 0;  // result buffer pointer, the LRU one
    struct buf *pbp = 0; // prev pointer of result buffer
    struct buf *pb;      // prev pointer of b

#ifdef VERBOSE
    printf("bget: bucket[%d]\n", p);
    for(b = bcache.bucket[p].head.next; b != 0; b = b->next)
      printf("\t%p %d\n", (uint64)b, b->refcnt);
#endif

    for(b = bcache.bucket[p].head.next, pb = &bcache.bucket[p].head;
        b != 0; b = b->next, pb = pb->next)
      if(b->refcnt == 0){
#ifdef VERBOSE
        printf("bget: find buf whose refcnt = 0\n");
#endif
        if(bp == 0 || bp->ticks < b->ticks){
          bp = b;
          pbp = pb;
          break;
        }
      }

    // find the LRU unused buf
    if(bp != 0){
#ifdef DEBUG
      printf("bget: move %p from bucket[%d] to bucket[%d]\n", (uint64)bp, p, i);
#endif
      bp->ticks = ticks;
      bp->dev = dev;
      bp->blockno = blockno;
      bp->valid = 0;
      bp->refcnt = 1;
      pbp->next = bp->next; // remove bp from bucket[p]
      release(&bcache.bucket[p].lock);

      acquire(&bcache.bucket[i].lock);
      bp->next = bcache.bucket[i].head.next; // insert bp into bucket[i]
      bcache.bucket[i].head.next = b;
      release(&bcache.bucket[i].lock);
      acquiresleep(&bp->lock);
      return bp;
    }

    release(&bcache.bucket[p].lock);

    p = (p + 1) % NBUCKET;
    if(p == i) break; // walk through all bufs, but not find
  }
#ifdef DEBUG
  printf("bget: bucket[%d]\n", i);
#endif
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int i = hash(b->blockno);

  acquire(&bcache.bucket[i].lock);
  if(b->refcnt == 0) panic("brelse: release a un-ref buf");
  b->refcnt--;

#ifdef DEBUG
  printf("breles: release %p; refcnt: %d\n", (uint64)b, b->refcnt);
#endif
  release(&bcache.bucket[i].lock);
}

void
bpin(struct buf *b) {
  int i = hash(b->blockno);
  acquire(&bcache.bucket[i].lock);
  b->refcnt++;
  release(&bcache.bucket[i].lock);
}

void
bunpin(struct buf *b) {
  int i = hash(b->blockno);
  acquire(&bcache.bucket[i].lock);
  b->refcnt--;
  release(&bcache.bucket[i].lock);
}


