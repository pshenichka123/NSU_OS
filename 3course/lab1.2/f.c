
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define SUCCESS 0
#define ERROR -1

void *thread_func(void *arg)
{
    printf("Идентификатор потока: %lu\n", (unsigned long)pthread_self());
    return NULL;
}

int main()
{
    int counter = 0;
    pthread_attr_t attr;
    int result = pthread_attr_init(&attr);
    if (result != SUCCESS)
    {
        perror("pthread_attr_init");
        return ERROR;
    }

    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (result != SUCCESS)
    {
        perror("pthread_attr_setdetachstate");
        pthread_attr_destroy(&attr);
        return ERROR;
    }

    while (true)
    {
        pthread_t thread;
        result = pthread_create(&thread, &attr, thread_func, NULL);
        if (result != SUCCESS)
        {
            perror("pthread_create");
            break;
        }
        ++counter;
        printf("thread count: %d\n", counter);
    }
    pthread_attr_destroy(&attr);
    return SUCCESS;
}