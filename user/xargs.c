// xargs.c - util exercise 5
//         - Huang 2022-09-05
#include "kernel/types.h"
// to prevent vsc format
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

char* readline();
int parseline(char* line, char* xargv[], int argc);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [args]\n");
        exit(1);
    }

    // init stuff for args
    char* xargv[MAXARG];
    char* cmd = argv[1];
    xargv[0] = argv[1];  // command is the first arg
    for (int i = 2; i < argc; i++)
        // for initial args
        xargv[i - 1] = argv[i];

    char* line;
    while ((line = readline()) != 0) {
        parseline(line, xargv, argc);

        if (fork() == 0)
            exec(cmd, xargv);
        wait(0);
        free(line);
    }

    exit(0);
}

char* readline() {
    char* buf = (char*)malloc(64 * MAXARG);
    char* p = buf;
    while (read(0, p, 1) == 1) {
        if (*p == '\0' || *p == '\n') {
            *p = '\0';
            return buf;
        }
        p++;
    }

    free(buf);
    return 0;  // nothing happens, aka read nothing
}

int parseline(char* line, char* xargv[], int argc) {
    int xcnt = argc - 1;
    char *p, *arg = line;
    for (p = line; *p != '\0'; p++) {
        if (*p == ' ') {
            *p = '\0';
            xargv[xcnt++] = arg;  // args end with ' '
            arg = ++p;            // skip '\0'
        }
        if (xcnt == MAXARG && *p != '\0') {
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
        }
    }
    xargv[xcnt++] = arg;  // arg end with '\n' or '\0'

    return xcnt;
}
