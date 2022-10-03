# cow

> [cow](https://pdos.csail.mit.edu/6.S081/2020/labs/cow.html) lab will add a
> feature to xv6: Copy-on-Write fork().

## Prepare

The goal of copy-on-write (COW) fork() is to defer allocating and copying
physical memory pages for the child until the copies are actually needed, if
ever.

## COW

There two main jobs we need to do:
1. Handle page fault on a cow page
2. Maintain reference count to each page

* Prototype
For COW mapping, use RSW in PTE to record.
For reference count, maintain a fixed-size array of indexes to page.

Plan of guide:
1. `uvmcopy`: map parent's physical page to child, clear PTE_W and set PTE_C
   for both child and parent
2. `usertrap`: handle store page fault on COW page, copy the old to the new and
   install it in PTE with PTE_W set and PTE_C clear
