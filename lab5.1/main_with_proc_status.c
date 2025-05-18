#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#define ERROR -1
#define CHILD 0
int global_var = 10;







void print_proc_status(int pid)
{

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

void Child_Process(int* local_var)
{

    global_var = 100;
    *local_var = 200;

    sleep(2);
}

int main() {
    int local_var = 20;


    pid_t pid = fork();

    if (pid == ERROR) {
        perror("fork failed");
        return 0;;
    }
    if (pid == CHILD) {
        Child_Process(&local_var);
        _exit(5);
    }
    print_proc_status(pid);


    sleep(1);

    print_proc_status(pid);

    sleep(4);
    print_proc_status(pid);
    int status;
    pid_t child_pid = wait(&status);

    if (child_pid == ERROR) {
        perror("wait failed");
        exit(1);
    }

    sleep(1);
    bool executed_correctly = WIFEXITED(status);
    if (executed_correctly) {
        printf("Child process %d exited with status: %d\n", child_pid, WEXITSTATUS(status));
    }

    bool executed_by_signal = WIFSIGNALED(status);
    if (executed_by_signal) {
        printf("Child process %d was killed by signal: %d\n", child_pid, WTERMSIG(status));
    }


    return 0;
}