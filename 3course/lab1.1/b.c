#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#define SUCCESS 0
#define THREAD_COUNT 5
void *mythread(void *arg)
{
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    return NULL;
}

int main()
{
    pthread_t tid[THREAD_COUNT];
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        err = pthread_create(&tid[i], NULL, mythread, NULL);
        if (err != SUCCESS)
        {
            fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (pthread_join(tid[i], NULL) != SUCCESS)
        {
            fprintf(stderr, "main: pthread_join() failed: %s\n", strerror(err));
        }
    }

    pthread_exit(NULL);
}