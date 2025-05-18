#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
int global_initialized = 10;
int global_uninitialized;
const int global_const = 20;




void print_proc_status()
{
    pid_t pid = getpid();

    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Failed to open /proc/<pid>/status");
        return;
    }

    char buff[256];
    while (true) {
        char* read_line = fgets(buff, sizeof(buff), file);
        if (read_line == NULL) {
            if (!feof(file)) {
                perror("Error reading file");
                break;
            }
            printf("Reached end of file\n");
            break;
        }

        printf("%s", buff);
    }

    fclose(file);

    return;
}



void set_env_var(const char* env_var_name) {

    char* env_value = getenv(env_var_name);
    if (env_value == NULL) {
        printf("Environment variable '%s' is not set.\n", env_var_name);
        return;
    }
    printf("Initial value of '%s': %s\n", env_var_name, env_value);


    const char* new_value = "new_value";
    if (setenv(env_var_name, new_value, 1) != 0) {
        perror("Failed to set environment variable");
        return;
    }

    env_value = getenv(env_var_name);
    if (env_value == NULL) {
        printf("Environment variable '%s' is not set.\n", env_var_name);
        return;
    }

    printf("Updated value of '%s': %s\n", env_var_name, env_value);


    return;
}

void buffer_operations() {
    char* buffer1 = (char*)malloc(1024);
    if (buffer1 == NULL) {
        perror("Failed to allocate buffer2");
        return;
    }

    strcpy(buffer1, "hello world");
    printf("Buffer content after allocation: %s\n", buffer1);

    free(buffer1);
    printf("Buffer content after free: %s\n", buffer1);

    char* buffer2 = (char*)malloc(1024);
    if (buffer2 == NULL) {
        perror("Failed to allocate buffer2");
        return;
    }
    strcpy(buffer2, "hello world again");
    printf("New buffer content: %s\n", buffer2);

    char* mid_ptr = buffer2 + 50;
    free(mid_ptr);
    printf("New buffer content after free: %s\n", buffer2);
}



void print_local_vars() {

    int local_var = 30;
    static int static_var = 40;
    const int local_const = 50;
    printf("Local variable address: %p\n", (void*)&local_var);
    printf("Static variable address: %p\n", (void*)&static_var);
    printf("Local constant address: %p\n", (void*)&local_const);
}
void print_global_vars()
{
    printf("Global initialized variable address: %p\n", (void*)&global_initialized);
    printf("Global uninitialized variable address: %p\n", (void*)&global_uninitialized);
    printf("Global constant address: %p\n", (void*)&global_const);
}


void print_proc_maps()
{
    pid_t pid = getpid();

    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Failed to open /proc/<pid>/maps");
        return;
    }

    char buff[256];
    while (true) {
        char* read_line = fgets(buff, sizeof(buff), file);
        if (read_line == NULL) {
            if (!feof(file)) {
                perror("Error reading file");
                break;
            }
            printf("Reached end of file\n");
            break;
        }

        printf("%s", buff);
    }

    fclose(file);

    return;
}


int main(int argc, char** argv) {

    if (argc < 2) {
        puts("Pass parameter: enviroment name");
        return 0;
    }
    print_local_vars();
    print_global_vars();
    //print_proc_maps();
    //buffer_operations();

    set_env_var(argv[1]);

    return 0;
}