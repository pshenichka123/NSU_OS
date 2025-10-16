#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define ERROR -1

void *thread_func()
{
    printf("Поток создан и возвращает %d.\n", 42);
    return (void *)42;
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

    int returned_value = *(int *)ret_val;
    printf("Основной поток: получено значение %d.\n", returned_value);
    return SUCCESS;
}