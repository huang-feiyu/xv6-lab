// pingpong.c - util exercise 2
//            - Huang 2022-09-04
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid;
    int p_fd[2], c_fd[2];  // 0 for read, 1 for write
    char buf[5];

    pipe(p_fd);
    pipe(c_fd);

    pid = fork();
    if (pid > 0) {
        write(p_fd[1], "ping", 4);
        read(c_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
    } else {
        read(p_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
        write(c_fd[1], "pong", 4);
    }

    close(p_fd[0]);
    close(p_fd[1]);
    close(c_fd[0]);
    close(c_fd[1]);

    exit(0);
}
