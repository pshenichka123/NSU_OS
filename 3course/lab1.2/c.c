#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define ERROR -1

void *thread_func()
{

    printf("Поток создан и возвращает строку.\n");
    return (void *)"hello world";
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
    char *returned_str = (char *)ret_val;
    printf("Основной поток: получена строка \"%s\".\n", returned_str);
    return SUCCESS;
}