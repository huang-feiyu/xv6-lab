# Chapter 8

> File System

* Challenge
  * Representation
  * Crash Recovery
  * Multi-Thread
  * Maintain In-Mem cache
* Layers of xv6 File System: (Bottom up)
  * Disk -> Buffer cache -> Logging -> Inode -> Directory -> Pathname -> File Descriptor
* Disk Layer 在 virtio 磁盘上读写块
* Buffer Cache
  * 同步访问磁盘块，以确保磁盘块在内存中只有一个buffer缓存，并且一次只有一个内核线程能使用该buffer缓存
  * 缓存使用较多的块，这样它们就不需要从慢速磁盘中重新读取
* Logging Layer: TODO:
* Inode Layer (Read the code...)
  * 指的是磁盘上的数据结构，其中包含了文件的大小和数据块号的列表
  * 指的是内存中的 inode，它包含了磁盘上 inode 的副本以及内核中需要的其他信息。
* File Descriptor Layer
  * Each open file is represented by a struct file (kernel/file.h:1), which is a
    wrapper around either an inode or a pipe, plus an I/O offset.
  * most resources in Unix are represented as files, including devices such as
    the console, pipes, and of course, real files

---

MIT Lecture 14:

* 实际上操作系统内部需要对于文件有内部的表现形式，并且这种表现形式与文件名无关
* inode通常小于一个disk block，所以多个inode通常会打包存储在一个disk block中 (为了向单个inode提供同步操作，XV6维护了inode cache)
* Disk Layer
  * sector通常是磁盘驱动可以读写的最小单元，它过去通常是512字节
  * block通常是操作系统或者文件系统视角的数据，由文件系统定义（对应一个或多个sector，xv6中对应两个）
    * 多个inode会打包存在一个block中，一个inode是64字节 （只要有inode的编号，我们总是可以找到inode在磁盘上存储的位置）
  * bitmap block，inode blocks和log blocks被统称为metadata block
* Inode Layer: inode中的信息完全足够用来实现read/write系统调用
  * 12个direct block number编号指向了构成文件的前12个block。举个例子，如果文件只有2个字节，那么只会有一个block编号0，它包含的数字是磁盘上文件前2个字节的block的位置。
  * indirect block number，它对应了磁盘上一个block，这个block包含了256个block number，这256个block number包含了文件的数据
* Directory
  * 一个目录本质上是一个文件加上一些文件系统能够理解的结构
  * 一个目录包含了directory entries: 前两个字节为文件或子目录inode编号，后14字节为文件或子目录名
* mkfs: 创建了一个包含一些文件的新的文件系统/磁盘镜像

