#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

int writeFile(char *filePath, char *contents)
{
    int fd = open(filePath, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }

    flock(fd, LOCK_EX);

    size_t len = strlen(contents);
    char *p = contents;
    while(0 < len) {
        int size = write(fd, p, len);
        if (size == 0) {
            break;
        } else if (size < 0) {
            perror("write error");
            return -2;
        }

        len -= size;
        p += size;
    }

    fsync(fd);
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

int main(int argc, char **argv)
{
    // 無限に test.txt に書き込みを行う
    while(1)
    {
        int ret = writeFile("test.txt", "123ABC");
        if (ret < 0) {
            printf("writeWithoutLock failed...\n");
            break;
        }

        printf("test.txt write success...\n");
    }

    return 0;
}

