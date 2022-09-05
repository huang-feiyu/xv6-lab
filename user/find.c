// find.c - util exercise 4
//        - Huang 2022-09-05
#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char* pat, char* path);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(2, "Usage: find [dir|file ...] pattern\n");
        exit(1);
    }

    int i;
    char* pattern = argv[argc - 1];
    for (i = 1; i < argc - 1; i++) {
        find(pattern, argv[i]);
    }

    exit(0);
}

char* filename(char* buf) {
    int i;
    char* p = buf;
    for (i = 0; i < strlen(buf); i++)
        if (buf[i] == '/' && buf[i + 1] != 0)
            p = &buf[i + 1];
    return p;
}

void find(char* pat, char* path) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            if (strcmp(filename(path), pat) == 0)
                printf("%s\n", path);
            break;

        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                fprintf(2, "find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue;
                if (de.name[0] == '.' && de.name[1] == 0) continue;
                if (de.name[0] == '.' && de.name[1] == '.' && de.name[2] == 0) continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (stat(buf, &st) < 0) {
                    fprintf(2, "find: cannot stat %s\n", buf);
                    continue;
                }
                find(pat, buf);
            }
            break;
    }

    close(fd);
}
