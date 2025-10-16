
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
    printf("data: %d ,%s\n", data->digit, data->str);
    free(data->str);
    free(data);
    return NULL;
}

int init_detached(pthread_attr_t *attr)
{
    int result = pthread_attr_init(attr);
    if (result != SUCCESS)
    {
        perror("pthread_attr_init");
        return ERROR;
    }

    result = pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
    if (result != SUCCESS)
    {
        perror("pthread_attr_setdetachstate");
        pthread_attr_destroy(attr);
        return ERROR;
    }
    return SUCCESS;
}
int main()
{
    struct my_struct *data = (struct my_struct *)malloc(sizeof(struct my_struct));
    if (data == NULL)
    {
        perror("malloc");
        return ERROR;
    }
    data->digit = 5;
    char *message = "string";
    data->str = (char *)malloc(sizeof(char) * (strlen(message) + 1));
    strcpy(data->str, message);
    if (data->str == NULL)
    {
        perror("malloc");
        return ERROR;
    }

    pthread_attr_t attr;

    pthread_t thread;
    int result = init_detached(&attr);
    if (result != SUCCESS)
    {
        perror("init_detached");
        pthread_attr_destroy(&attr);
        return ERROR;
    }
    result = pthread_create(&thread, &attr, thread_func, data);
    if (result != SUCCESS)
    {
        perror("pthread_create");
        pthread_attr_destroy(&attr);
        return ERROR;
    }
    result = pthread_attr_destroy(&attr);
    if (result != SUCCESS)
    {
        perror("pthread_attr_destroy");
        return ERROR;
    }
    pthread_exit(NULL);
}