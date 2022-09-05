# util

> [util](https://pdos.csail.mit.edu/6.S081/2021/labs/util.html) lab will
> familiarize you with xv6 and its system calls.

### sleep

Follow the guide.

### pingpong

Create 2 pipes one for parent/child read&write.

### primes

Create an array contains 2~35, then follow the pseudocode to sieve it.

![sieve.gif](https://swtch.com/~rsc/thread/sieve.gif)

```pseudocode
p = get a number from left neighbor
    if (p does not divide n)
        send n to right neighbor
print p
loop:
    n = get a number from left neighbor
    if (p does not divide n)
        send n to right neighbor
```

### find

find dir name

Recursion to each sub-directory, just need to follow the [ls.c](./user/ls.c) to
implement `find`.

### xargs

We need to implement `xargs` which behaves like `xargs -n 1` in your UNIX shell:

```bash
$ echo -e "1\n2" | xargs -n 1 echo line
line 1 # output
line 2
```

---

```
Time:   5 Hours
Score:  100/100
Author: Huang
Date:   2022-09-05
```
