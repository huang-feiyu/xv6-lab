# fs

> lab 9: Report

## Analysis on content

This lab contains two tasks:
* Add a double-indirect block to support large file<br/>
  The task tells us what `inode` is and how it is organized
* Add soft link to xv6: Add a syscall `symlink` and some process to `fileopen`<br/>
  soft link can cross disk devices, is a linked file by pathname

## Large files: Design and Analysis

What we need to do is to translate the picture below to code:

<img src="https://user-images.githubusercontent.com/70138429/194250950-4673cc49-73cc-44f6-a121-e163fb4891d3.png" width="500px"></img>

1. Change prototype of `inode`, `dinode` and relevant macros for big files
2. `bmap`: Translate **logical block** bn to **disk block** (alloc if none)<br/>
   Check bn orderly to find out what type it is<br/>
   (1) If is direct/indirect, then done by previous code<br/>
   (2) Otherwise, use modulo and fraction to find what the indirect block is and
       the data block
3. `itrunc`: Free up allocated blocks. Just walk through every block orderly and
  free it, no magic here.

## Symbolic links: Design and Analysis

In this lab, we are going to add a feature to xv6: soft link. There are 3 main
jobs we need to deal with:
* Add new file type T_SYMLINK & store target
* Create symlink file
* Open symlink file

0. Add prototypes following guide
1. `symlink`: Creates a symbolic `link` which contains string `target`<br/>
   (1) Create file with type T_SYMLINK<br/>
   (2) Write target to file, just write to the top
2. `open`: Handle open symlink file without O_NOFOLLOW<br/>
   (1) Read file from the top => extract target<br/>
   (2) Open target until it is not a symlink file
      (Only one thing to mention here: if cycle too many times, force stopping)

---

> The following is note while doing lab.

# fs

> In [fs](https://pdos.csail.mit.edu/6.S081/2020/labs/fs.html) lab you will add
> large files and symbolic links to the xv6 file system.

## big file

Change one direct block to doubly-direct block.

Just do stuff like indirect block twice, it is fairly easy to implement.

## symbolic link

Add one more syscall `symlink`.

I solved many bugs, most of them were not worth recording.

---

```
Time:   5 Hours
Score:  100/100
Author: Huang
Date:   2022-09-19
```
