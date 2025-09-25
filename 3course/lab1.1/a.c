#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#define SUCCESS 0
void *mythread(void *arg)
{
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    return NULL;
}

int main()
{
    pthread_t tid;
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err != SUCCESS)
    {
        fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err));
        pthread_exit(NULL);
    }

    pthread_exit(NULL);
}