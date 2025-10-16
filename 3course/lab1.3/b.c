
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

void end_routine(char *str, struct my_struct *data, pthread_attr_t *attr)
{
    perror(str);
    free(data->str);
    free(data);
    pthread_attr_destroy(attr);
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
    if (data->str == NULL)
    {
        free(data);
        perror("malloc");
        return ERROR;
    }
    strcpy(data->str, message);
    pthread_attr_t attr;
    pthread_t thread;
    int result = init_detached(&attr);
    if (result != SUCCESS)
    {
        end_routine("init_detached", data, &attr);
        return ERROR;
    }
    result = pthread_create(&thread, &attr, thread_func, data);
    if (result != SUCCESS)
    {
        end_routine("pthread_create", data, &attr);
        return ERROR;
    }
    result = pthread_attr_destroy(&attr);
    if (result != SUCCESS)
    {
        perror("pthread_attr_destroy");
        pthread_exit(NULL);
    }
    pthread_exit(NULL);
}