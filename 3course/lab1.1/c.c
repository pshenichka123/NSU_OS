#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#define SUCCESS 0
#define THREAD_COUNT 5
int global_var = 0;

void *mythread(void *arg)
{
    int local_var = 0;
    static int static_local_var = 0;
    const int const_local_var = 0;

    printf("mythread [%d %d %d %ld]: Hello from mythread!\n",
           getpid(), getppid(), gettid(), pthread_self());
    printf("Addresses: local=%p, static_local=%p, const_local=%p, global=%p\n",
           &local_var, &static_local_var, &const_local_var, &global_var);

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

    pthread_exit(NULL);
}