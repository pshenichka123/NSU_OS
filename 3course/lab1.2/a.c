#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define ERROR -1

void *thread_func()
{
    printf("Поток создан и завершается.\n");
    return NULL;
}

int main()
{
    pthread_t thread;
    int result = pthread_create(&thread, NULL, thread_func, NULL);
    if (result != SUCCESS)
    {
        perror("pthread_create");
        return ERROR;
    }

    void *ret_val;
    result = pthread_join(thread, &ret_val);
    if (result != SUCCESS)
    {
        perror("pthread_join");
        return ERROR;
    }
    printf("Основной поток: дочерний поток завершился.\n");
    return SUCCESS;
}