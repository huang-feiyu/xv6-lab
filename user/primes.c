// primes.c - util exercise 3
//          - Huang 2022-09-05
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int fd);

int main(int argc, char *argv[]) {
    int i;
    int parent_fd[2];  // 0 for read, 1 for write

    pipe(parent_fd);

    if (fork()) {
        close(parent_fd[0]);
        for (i = 2; i < 36; i++)
            write(parent_fd[1], &i, sizeof(int));
        close(parent_fd[1]);
    } else {
        close(parent_fd[1]);
        sieve(parent_fd[0]);
        // close parent_fd[0] in sieve
    }

    wait(0);
    exit(0);
}

// sieve - sieve the nums by mod n
void sieve(int fd) {
    int n;
    // read the first number from left
    if (read(fd, &n, sizeof(int)) == 0) {
        close(fd);
        exit(0);
    }
    printf("prime %d\n", n);

    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        close(pipefd[1]);
        sieve(pipefd[0]);
    } else {
        close(pipefd[0]);
        int num;
        while (read(fd, &num, sizeof(int)))
            if (num % n != 0)
                write(pipefd[1], &num, sizeof(int));
        close(pipefd[1]);
    }
    close(fd);

    wait(0);
    exit(0);
}
