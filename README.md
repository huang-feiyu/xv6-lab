# fs

> In [fs](https://pdos.csail.mit.edu/6.S081/2020/labs/fs.html) lab you will add
> large files and symbolic links to the xv6 file system.

## big file

Change one direct block to doubly-direct block.

Just do stuff like indirect block twice, it is fairly easy to implement.

## symbolic link

Add one more syscall `symlink`.

I solved many bugs, most of them were not worth to record.
