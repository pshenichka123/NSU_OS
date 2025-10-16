#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#define SUCCESS 0

void *thread_func()
{
    printf("Идентификатор потока: %lu\n", (unsigned long)pthread_self());
    return NULL;
}

int main()
{
    int counter = 0;
    while (true)
    {
        pthread_t thread;
        int result = pthread_create(&thread, NULL, thread_func, NULL);
        if (result != SUCCESS)
        {
            perror("pthread_create");
            break;
        }
        ++counter;
    }
    printf("thread count: %d\n", counter);
    getchar();

    return SUCCESS;
}