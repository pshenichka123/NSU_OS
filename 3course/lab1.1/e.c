#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define SUCCESS 0
#define THREAD_COUNT 5

void *mythread(void *arg)
{
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());

    sleep(2);
    return NULL;
}

int main()
{
    pthread_t tid[THREAD_COUNT];
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
    printf("Process PID: %d\n", getpid());
    printf("Check /proc/%d/maps for thread stacks\n", getpid());

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        err = pthread_create(&tid[i], NULL, mythread, NULL);
        if (err != SUCCESS)
        {
            fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err));
            pthread_exit(NULL);
        }
    }

    sleep(100);
    pthread_exit(NULL);
}