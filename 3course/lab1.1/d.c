#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 0;

void *mythread(void *arg) {
    int local_var = 0;
    
    printf("Before: local_var=%d, global_var=%d\n", local_var, global_var);
    
    local_var++;
    global_var++;
    
    printf("After: local_var=%d, global_var=%d\n", local_var, global_var);
    
    return NULL;
}

int main() {
    pthread_t tid[5];
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for (int i = 0; i < 5; i++) {
        err = pthread_create(&tid[i], NULL, mythread, NULL);
        if (err) {
            fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("Final global_var=%d\n", global_var);
    return 0;
}