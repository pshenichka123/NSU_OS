
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SUCCESS 0
#define ERROR -1
struct my_struct
{
    int digit;
    char *str;
};

void *thread_func(void *attr)
{
    struct my_struct *data = (struct my_struct *)attr;
    printf("data: %d ,   %s\n", data->digit, data->str);
    return NULL;
}

int main()
{
    struct my_struct data;

    data.digit = 5;
    data.str = "string";
    pthread_t thread;
    int result = pthread_create(&thread, NULL, thread_func, &data);
    if (result != SUCCESS)
    {
        perror("pthread_create");
        return ERROR;
    }
    result = pthread_join(thread, NULL);
    if (result != SUCCESS)
    {
        perror("pthread_join");
        return ERROR;
    }

    return SUCCESS;
}