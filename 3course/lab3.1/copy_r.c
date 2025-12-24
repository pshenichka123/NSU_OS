#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#define SUCCESS 0
#define ERROR -1
#define CURENT_DIRECTORY_RELATIVE_PATH "."
#define PARENT_DIRECTORY_RELATIVE_PATH ".."
#define EQUAL 0
#define ARG_COUNT 3
#define MAX_TRY_COUNT 120
#define PERMISSIONS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
void* process_directory(void* arg);

struct copy_task
{
    char sourcePath[PATH_MAX];
    char dstPath[PATH_MAX];
};

int safe_pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg)
{
    int try_count = 0;
    int status = pthread_create(thread, attr, start_routine, arg);
    while (status != SUCCESS && try_count < MAX_TRY_COUNT)
    {
        if (status == EAGAIN)
        {
            sleep(1);
            status = pthread_create(thread, attr, start_routine, arg);
            try_count++;
            continue;
        }
        return status;
    }
    if (try_count >= MAX_TRY_COUNT)
    {
        return ERROR;
    }
    return SUCCESS;
}

int open_source_file(const char* path)
{
    int try_count = 0;
    int fd = open(path, O_RDONLY);
    while (fd == ERROR && try_count < MAX_TRY_COUNT)
    {
        if (errno == EMFILE)
        {
            sleep(1);
            fd = open(path, O_RDONLY);
            try_count++;
            continue;
        }
        printf("%s\n", path);
        perror("open source file:\n");
        return ERROR;
    }
    if (try_count >= MAX_TRY_COUNT)
    {
        return ERROR;
    }
    return fd;
}

int open_dest_file(const char* path)
{
    int try_count = 0;
    int fd = open(path, O_WRONLY | O_CREAT, PERMISSIONS);
    while (fd == ERROR && try_count < MAX_TRY_COUNT)
    {
        if (errno == EMFILE)
        {
            sleep(1);
            fd = open(path, O_WRONLY | O_CREAT, PERMISSIONS);
            try_count++;
            continue;
        }
        printf("%s\n", path);
        perror("open destination file");
        return ERROR;
    }
    if (try_count >= MAX_TRY_COUNT)
    {
        return ERROR;
    }
    return fd;
}

void copy_file_data(int src_fd, int dst_fd)
{
    char buffer[4096];
    ssize_t bytes_read = 0;

    bytes_read = read(src_fd, buffer, sizeof(buffer));
    while (bytes_read > 0)
    {
        ssize_t total_written = 0;

        while (total_written < bytes_read)
        {
            ssize_t written = write(dst_fd,
                buffer + total_written,
                bytes_read - total_written);
            if (written == ERROR)
            {
                perror("write");
                return;
            }
            total_written += written;
        }

        bytes_read = read(src_fd, buffer, sizeof(buffer));
    }

    if (bytes_read == ERROR)
        perror("read");
}

void copy_file_permissions(const char* src_path, int dst_fd)
{
    struct stat src_stat;
    int res = stat(src_path, &src_stat);
    if (res != SUCCESS)
        perror("stat");
    fchmod(dst_fd, src_stat.st_mode);
}

void* copy_file(void* arg)
{
    struct copy_task* task = arg;

    int src_fd = open_source_file(task->sourcePath);
    if (src_fd == ERROR)
    {
        free(task);
        return NULL;
    }

    int dst_fd = open_dest_file(task->dstPath);
    if (dst_fd == ERROR)
    {
        close(src_fd);
        free(task);
        return NULL;
    }

    copy_file_data(src_fd, dst_fd);
    copy_file_permissions(task->sourcePath, dst_fd);

    close(src_fd);
    close(dst_fd);
    free(task);
    return NULL;
}

void join_path(const char* dir, const char* file, char* out, size_t out_size)
{
    size_t len = snprintf(out, out_size, "%s/%s", dir, file);
    if (len >= out_size)
        fprintf(stderr, "path too long: %s/%s\n", dir, file);
}

int count_directory_entries(DIR* dir)
{
    struct dirent* entry;
    int count = 0;

    rewinddir(dir);
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, CURENT_DIRECTORY_RELATIVE_PATH) && strcmp(entry->d_name, PARENT_DIRECTORY_RELATIVE_PATH))
            count++;
    }

    rewinddir(dir);
    return count;
}

void join_all_threads(pthread_t* threads, int thread_count)
{
    for (int thread_index = 0; thread_index < thread_count; thread_index++)
    {
        int status = pthread_join(threads[thread_index], NULL);
        if (status != SUCCESS)
            perror("pthread_join failed");
    }
}

void copy_directory_permissions(const char* src, const char* dst)
{
    struct stat st;
    int res = stat(src, &st);
    if (res == SUCCESS)
        chmod(dst, st.st_mode);
}

int open_directory(const char* path, DIR** dir)
{
    *dir = opendir(path);
    while (*dir == NULL)
    {
        if (errno == EMFILE)
        {
            sleep(1);
            *dir = opendir(path);
            continue;
        }
        perror("opendir");
        return ERROR;
    }
    return SUCCESS;
}

void handle_entry(struct copy_task* task, struct dirent* entry, pthread_t* threads, int* thread_count)
{
    char src[PATH_MAX], dst[PATH_MAX];
    join_path(task->sourcePath, entry->d_name, src, PATH_MAX);
    join_path(task->dstPath, entry->d_name, dst, PATH_MAX);

    struct stat st;
    int st_res = lstat(src, &st);
    if (st_res == ERROR)
    {
        perror("lstat");
        return;
    }

    struct copy_task* new_task = malloc(sizeof(*new_task));
    strncpy(new_task->sourcePath, src, PATH_MAX);
    strncpy(new_task->dstPath, dst, PATH_MAX);

    int status;
    if (S_ISREG(st.st_mode))
        status = safe_pthread_create(&threads[*thread_count], NULL, copy_file, new_task);
    else if (S_ISDIR(st.st_mode))
        status = safe_pthread_create(&threads[*thread_count], NULL, process_directory, new_task);
    else
    {
        free(new_task);
        return;
    }

    if (status != SUCCESS)
    {
        free(new_task);
        return;
    }
    (*thread_count)++;

    return;
}
void* process_directory(void* arg)
{
    struct copy_task* task = arg;

    int status = mkdir(task->dstPath, PERMISSIONS);
    if (status == ERROR && errno != EEXIST)
    {
        printf("%s", task->dstPath);

        perror("mkdira");
        free(task);
        return NULL;
    }

    DIR* dir;
    status = open_directory(task->sourcePath, &dir);
    if (status == ERROR)
    {
        free(task);
        return NULL;
    }

    int file_count = count_directory_entries(dir);

    pthread_t* threads = malloc(sizeof(pthread_t) * file_count);
    if (threads == NULL)
    {
        perror("malloc:");
        return NULL;
    }
    int thread_count = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, CURENT_DIRECTORY_RELATIVE_PATH) && strcmp(entry->d_name, PARENT_DIRECTORY_RELATIVE_PATH))
            handle_entry(task, entry, threads, &thread_count);
    }

    join_all_threads(threads, thread_count);
    copy_directory_permissions(task->sourcePath, task->dstPath);

    free(threads);
    closedir(dir);
    free(task);
    return NULL;
}





static int validate_and_prepare_inputs(int argc, char* argv[], char* source_dir, char* dest_dir) {
    if (argc != ARG_COUNT) {
        fprintf(stderr, "Usage: %s source_dir dest_dir\n", argv[0]);
        return ERROR;
    }

    char real_src[PATH_MAX], real_dst[PATH_MAX];
    char* src_arg = argv[1];
    char* dst_arg = argv[2];
    int status;
    if (realpath(src_arg, real_src) == NULL)
    {
        perror("realpath source");
        return ERROR;
    }

    struct stat st;
    status = stat(real_src, &st);
    if (status == ERROR || !S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "Source is not a directory\n");
        return ERROR;
    }
    status = mkdir(dst_arg, PERMISSIONS);
    if (status == ERROR && errno != EEXIST)
    {
        perror("mkdir dest");
        return ERROR;
    }

    if (realpath(dst_arg, real_dst) == NULL)
    {
        perror("realpath dest");
        return ERROR;
    }

    if (!strncmp(real_src, real_dst, strlen(real_src)) && (real_dst[strlen(real_src)] == '/' || real_dst[strlen(real_src)] == '\0'))
    {
        fprintf(stderr, "Dest is inside source\n");
        return ERROR;
    }
    status = strcmp(real_src, real_dst);
    if (status == EQUAL)
    {
        fprintf(stderr, "Source and dest are same\n");
        return ERROR;

    }
    strncpy(source_dir, real_src, PATH_MAX);
    strncpy(dest_dir, real_dst, PATH_MAX);
    return SUCCESS;
}


int main(int argc, char* argv[]) {
    char source_dir[PATH_MAX], dest_dir[PATH_MAX];

    if (validate_and_prepare_inputs(argc, argv, source_dir, dest_dir) != SUCCESS)
        return ERROR;


    struct copy_task* root = malloc(sizeof(*root));
    if (root == NULL) {
        perror("malloc");
        return ERROR;
    }
    strncpy(root->sourcePath, source_dir, PATH_MAX);
    strncpy(root->dstPath, dest_dir, PATH_MAX);


    process_directory(root);
    return SUCCESS;
}
