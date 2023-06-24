#include <semaphore.h>

int main(int argc, char **argv)
{
    sem_unlink("/sem_lock");
}

