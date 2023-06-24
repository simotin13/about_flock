#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

int readFile(char *filePath, char *buf, size_t bufSize)
{
    // open前にsem_openを呼び出すことで ファイルアクセスを排他制御する
    sem_t *sem = sem_open("/sem_lock", O_CREAT, 0744, 1);
    sem_wait(sem);

    int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    size_t pos = 0;
    while(pos < bufSize) {
        size_t size = read(fd, &buf[pos], sizeof(buf));
        if (size == 0) {
            break;
        } else if (size < 0) {
            perror("read error");
            return -2;
        }

        pos += size;
    }

    fsync(fd);
    close(fd);

    // close後にsem_closeを呼び出すことで排他制御を解除する
    sem_post(sem);
    return 0;
}

int main(int argc, char **argv)
{
    // 無限に test.txt を読み込む
    while(1)
    {
        char buf[1024];
        memset(buf, 0, 1024);

        int ret = readFile("test.txt", buf, 1024);
        if (ret < 0) {
            printf("readWithoutLock failed...\n");
            break;
        }

        // 読み込んだ内容が "123ABC" でなければエラー(不整合が発生した)
        if (strcmp(buf, "123ABC") != 0) {
            // 書き込んだ内容と同じ文字列が読み込めていない...
            printf("read mismatch:[%s]\n", buf);
            break;
        }

        // 書き込んでいる内容(123ABC)が読み込めていれば成功
        printf("test.txt [%s] read success...\n", buf);
    }

    return 0;
}
