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

void *process_directory(void *arg);

struct copy_task
{
    char sourcePath[PATH_MAX];
    char dstPath[PATH_MAX];
};

int safe_pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    int res = pthread_create(thread, attr, start_routine, arg);
    while (res != 0)
    {
        if (res == EAGAIN)
        {
            sleep(1);
            res = pthread_create(thread, attr, start_routine, arg);
            continue;
        }
        return res;
    }
    return 0;
}

int open_source_file(const char *path)
{
    int fd = open(path, O_RDONLY);
    while (fd == -1)
    {
        if (errno == EMFILE)
        {
            sleep(1);
            fd = open(path, O_RDONLY);
            continue;
        }
        printf("%s\n", path);
        perror("open source file:\n");
        return -1;
    }
    return fd;
}

int open_dest_file(const char *path)
{
    int fd = open(path, O_WRONLY | O_CREAT, 0644);
    while (fd == -1)
    {
        if (errno == EMFILE)
        {
            sleep(1);
            fd = open(path, O_WRONLY | O_CREAT, 0644);
            continue;
        }
        printf("%s\n", path);
        perror("open destination file");
        return -1;
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
            ssize_t w = write(dst_fd,
                              buffer + total_written,
                              bytes_read - total_written);
            if (w == -1)
            {
                perror("write");
                return;
            }
            total_written += w;
        }

        bytes_read = read(src_fd, buffer, sizeof(buffer));
    }

    if (bytes_read == -1)
        perror("read");
}

void copy_file_permissions(const char *src_path, int dst_fd)
{
    struct stat src_stat;
    int res = stat(src_path, &src_stat);
    if (res == 0)
        fchmod(dst_fd, src_stat.st_mode);
}

void *copy_file(void *arg)
{
    struct copy_task *task = arg;

    int src_fd = open_source_file(task->sourcePath);
    if (src_fd == -1)
    {
        free(task);
        return NULL;
    }

    int dst_fd = open_dest_file(task->dstPath);
    if (dst_fd == -1)
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

void join_path(const char *dir, const char *file, char *out, size_t out_size)
{
    size_t len = snprintf(out, out_size, "%s/%s", dir, file);
    if (len >= out_size)
        fprintf(stderr, "path too long: %s/%s\n", dir, file);
}

int count_directory_entries(DIR *dir)
{
    struct dirent *entry;
    int count = 0;

    rewinddir(dir);
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            count++;
    }

    rewinddir(dir);
    return count;
}

void join_all_threads(pthread_t *threads, int count)
{
    for (int i = 0; i < count; i++)
    {
        int ret = pthread_join(threads[i], NULL);
        if (ret != 0)
            perror("pthread_join failed");
    }
}

void copy_directory_permissions(const char *src, const char *dst)
{
    struct stat st;
    int res = stat(src, &st);
    if (res == 0)
        chmod(dst, st.st_mode);
}

int open_directory(const char *path, DIR **dir)
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
        return -1;
    }
    return 0;
}

int handle_entry(struct copy_task *task, struct dirent *entry, pthread_t *threads, int *thread_count)
{
    char src[PATH_MAX], dst[PATH_MAX];
    join_path(task->sourcePath, entry->d_name, src, PATH_MAX);
    join_path(task->dstPath, entry->d_name, dst, PATH_MAX);

    struct stat st;
    int st_res = lstat(src, &st);
    if (st_res == -1)
    {
        perror("lstat");
        return 0;
    }

    struct copy_task *new_task = malloc(sizeof(*new_task));
    strncpy(new_task->sourcePath, src, PATH_MAX);
    strncpy(new_task->dstPath, dst, PATH_MAX);

    int cr = -1;
    if (S_ISREG(st.st_mode))
        cr = safe_pthread_create(&threads[*thread_count], NULL, copy_file, new_task);
    else if (S_ISDIR(st.st_mode))
        cr = safe_pthread_create(&threads[*thread_count], NULL, process_directory, new_task);
    else
    {
        free(new_task);
        return 0;
    }

    if (cr == 0)
        (*thread_count)++;
    else
        free(new_task);

    return 0;
}
void *process_directory(void *arg)
{
    struct copy_task *task = arg;

    int res = mkdir(task->dstPath, 0755);
    if (res == -1 && errno != EEXIST)
    {
        perror("mkdir");
        free(task);
        return NULL;
    }

    DIR *dir;
    res = open_directory(task->sourcePath, &dir);
    if (res == -1)
    {
        free(task);
        return NULL;
    }

    int file_count = count_directory_entries(dir);

    pthread_t *threads = malloc(sizeof(pthread_t) * file_count);
    int thread_count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            handle_entry(task, entry, threads, &thread_count);
    }

    join_all_threads(threads, thread_count);
    copy_directory_permissions(task->sourcePath, task->dstPath);

    free(threads);
    closedir(dir);
    free(task);
    return NULL;
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "%s source_dir dest_dir\n", argv[0]);
        return 1;
    }
    char real_src[PATH_MAX], real_dst[PATH_MAX];
    char *rp_res = realpath(argv[1], real_src);
    if (rp_res == NULL)
    {
        perror("realpath source");
        return 1;
    }
    struct stat st;
    int st_res = stat(real_src, &st);
    if (st_res == -1 || !S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "Source is not a directory\n");
        return 1;
    }
    int res = mkdir(argv[2], 0755);
    if (res == -1 && errno != EEXIST)
    {
        perror("mkdir dest");
        return 1;
    }
    rp_res = realpath(argv[2], real_dst);
    if (rp_res == NULL)
    {
        perror("realpath dest");
        return 1;
    }
    if (!strncmp(real_src, real_dst, strlen(real_src)) && (real_dst[strlen(real_src)] == '/' || real_dst[strlen(real_src)] == '\0'))
    {
        fprintf(stderr, "Dest is inside source\n");
        return 1;
    }
    struct copy_task *root = malloc(sizeof(*root));
    strncpy(root->sourcePath, real_src, PATH_MAX);
    strncpy(root->dstPath, real_dst, PATH_MAX);
    process_directory(root);
    return 0;
}
