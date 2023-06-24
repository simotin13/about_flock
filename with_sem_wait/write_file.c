#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <semaphore.h>

int writeFile(char *filePath, char *contents)
{
    sem_t *sem = sem_open("/sem_lock", O_CREAT, 0744, 1);
    sem_wait(sem);

    int fd = open(filePath, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }

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
    close(fd);

    // close後にsem_closeを呼び出すことで排他制御を解除する
    sem_post(sem);
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

