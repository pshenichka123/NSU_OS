#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define SUCCESS 0
#define ERROR -1

void *thread_func()
{
    printf("Идентификатор потока: %lu\n", (unsigned long)pthread_self());
    int result = pthread_detach(pthread_self());
    if (result != SUCCESS)
    {
        perror("pthread_detach");
    }
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

    return SUCCESS;
}
