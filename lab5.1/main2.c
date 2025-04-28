#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global_var = 10;

int main() {
    int local_var = 20;

    printf("Parent process - global_var: address=%p, value=%d\n", (void*)&global_var, global_var);
    printf("Parent process - local_var: address=%p, value=%d\n", (void*)&local_var, local_var);

    printf("Parent process - PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return 0;;
    }

    if (pid == 0) {
        printf("\nChild process - PID: %d\n", getpid());
        printf("Child process - Parent PID: %d\n", getppid());

        printf("Child process - global_var: address=%p, value=%d\n", (void*)&global_var, global_var);
        printf("Child process - local_var: address=%p, value=%d\n", (void*)&local_var, local_var);

        global_var = 100;
        local_var = 200;
        printf("Child process - changed global_var: value=%d\n", global_var);
        printf("Child process - changed local_var: value=%d\n", local_var);

        _exit(5); заменить на _exit
    }

    sleep(1);

    printf("\nParent process - global_var after child changes: value=%d\n", global_var);
    printf("Parent process - local_var after child changes: value=%d\n", local_var);

    int status;
    pid_t child_pid = wait(&status);

    if (child_pid == -1) {
        perror("wait failed");
        exit(1);
    }

    if (WIFEXITED(status)) {
        printf("Child process %d exited with status: %d\n", child_pid, WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        printf("Child process %d was killed by signal: %d\n", child_pid, WTERMSIG(status));
    }
    else {
        printf("Child process %d terminated abnormally\n", child_pid);
    }


    return 0;
}