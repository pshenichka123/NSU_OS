#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#define ERROR -1
#define DIR_ACCESS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define FILE_ACCESS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) 



int go_inside_dir(const char* dest_dir, const char* cur_dir_name, const char* src_path);

void reverse_buffer(char* buffer, int buffer_size)
{
    for (int i = 0; i < buffer_size / 2; i++)
    {
        char temp = buffer[i];
        buffer[i] = buffer[buffer_size - i - 1];
        buffer[buffer_size - i - 1] = temp;
    }
}




int get_file_size(int fd)
{
    int file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        fprintf(stderr, "Could't  set carette on end of file");
        perror("Error");
        return ERROR;
    }

    int behaviour_status = lseek(fd, 0, SEEK_SET);
    if (behaviour_status == -1) {
        fprintf(stderr, "Could't  return carette on fbegin of file");
        perror("Error");
        return ERROR;
    }
    return file_size;

}


int open_files(int* fdSrc, int* fdDest, const char* src_file, const char* dest_file)
{
    *fdSrc = open(src_file, O_RDONLY);
    if (*fdSrc == ERROR) {
        fprintf(stderr, "%s:%s", "Failed to open src file", src_file);
        perror("Error");
        return ERROR;
    }


    *fdDest = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, FILE_ACCESS);
    if (*fdDest == ERROR) {
        fprintf(stderr, "%s:%s", "Failed to open dest file", dest_file);
        perror("Error");
        return ERROR;
    }
    return 0;
}


int prepare_dest_path(const char* dest_dir, const char* file_name, char* dest_path)
{
    char reversed_name[PATH_MAX];
    strncpy(reversed_name, file_name, PATH_MAX - 1);
    int name_len = strlen(reversed_name);
    reverse_buffer(reversed_name, name_len);
    strncpy(dest_path, dest_dir, PATH_MAX - 1);
    int dest_path_len = strlen(dest_path);
    if ((dest_path_len + 1) >= PATH_MAX) {
        fprintf(stderr, "Path name is too long:%s/", dest_path);
        perror("Error");
        return ERROR;
    }
    strncat(dest_path, "/", PATH_MAX - strlen(dest_path) - 1);
    if ((dest_path_len + name_len) >= PATH_MAX)
    {
        fprintf(stderr, "Path name is too long:%s%s", dest_path, reversed_name);
        perror("Error");
        return ERROR;
    }
    strncat(dest_path, reversed_name, PATH_MAX - strlen(dest_path) - 1);
    return 0;
}

int reverse_file_content(const char* src_path, const char* dest_path)
{
    int fdSrc, fdDest;
    int behaviour_status = open_files(&fdSrc, &fdDest, src_path, dest_path);
    if (behaviour_status == ERROR) {
        return ERROR;
    }
    int file_size = get_file_size(fdSrc);
    if (file_size == -1)
    {
        fprintf(stderr, "%s:%s", "Errors while getting file size:", src_path);
        perror("Error");
        close(fdDest);
        close(fdSrc);
        return ERROR;
    }
    if (file_size == 0)
    {
        printf("file:%s   is empty\n", src_path);
    }

    const int buffer_size = 256;
    char buffer[buffer_size];
    int remaining_bytes = file_size;
    int offset = file_size;

    while (remaining_bytes > 0) {
        int bytes_to_read = (remaining_bytes > buffer_size) ? buffer_size : remaining_bytes;
        offset -= bytes_to_read;
        behaviour_status = lseek(fdSrc, offset, SEEK_SET);
        if (behaviour_status == ERROR) {
            break;
        }

        behaviour_status = read(fdSrc, buffer, bytes_to_read);
        if (behaviour_status != bytes_to_read) {
            behaviour_status = ERROR;
            break;
        }

        reverse_buffer(buffer, bytes_to_read);

        behaviour_status = write(fdDest, buffer, bytes_to_read);
        if (behaviour_status != bytes_to_read) {
            behaviour_status = ERROR;
            break;
        }
        remaining_bytes -= bytes_to_read;
    }

    if (behaviour_status == ERROR)
    {
        fprintf(stderr, "%s:%s", "Errors during file revercing file content:", src_path);
        perror("Error");
        close(fdSrc);
        close(fdDest);
        return ERROR;
    }
    close(fdSrc);
    close(fdDest);
    return 0;
}





int process_file(const char* dest_dir, const char* file_name, const char* src_path)
{
    char dest_path[PATH_MAX];
    prepare_dest_path(dest_dir, file_name, dest_path);
    int correctly_processed = reverse_file_content(src_path, dest_path);
    if (correctly_processed != 0) {
        return ERROR;
    }
    return 0;

}

int process_directory(const char* src_dir, const char* dest_dir)
{
    DIR* dir = opendir(src_dir);
    if (dir == NULL) {
        fprintf(stderr, "%s:%s", "Failed to open src dir:", src_dir);
        perror("Error");
        return ERROR;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {

        char src_path[PATH_MAX];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
        struct stat path_stat;
        if (lstat(src_path, &path_stat) != 0) {
            fprintf(stderr, "%s:%s", "Failed to get file stat:", src_dir);
            perror("Error");
            closedir(dir);
            return ERROR;
        }
        int behaviour_status = 0;

        if (S_ISDIR(path_stat.st_mode)) {
            behaviour_status = go_inside_dir(dest_dir, entry->d_name, src_path);
        }

        if (S_ISREG(path_stat.st_mode)) {
            behaviour_status = process_file(dest_dir, entry->d_name, src_path);
        }

        if (behaviour_status != 0) {
            closedir(dir);
            return ERROR;
        }

    }
    closedir(dir);
    return 0;
}

int go_inside_dir(const char* dest_dir, const char* cur_dir_name, const char* src_path)
{
    if (cur_dir_name[0] == '.') {
        return 0;
    }
    char dest_path[PATH_MAX];
    int behaviour_status = prepare_dest_path(dest_dir, cur_dir_name, dest_path);
    if (behaviour_status == ERROR)
    {
        fprintf(stderr, "Can't prepare destinanion path:%s/%s", dest_dir, cur_dir_name);
        perror("Error");
        return ERROR;
    }
    behaviour_status = mkdir(dest_path, DIR_ACCESS);
    if (behaviour_status != 0)
    {
        fprintf(stderr, "%s:%s", "Cant create directory:", dest_path);
        perror("Error");
        return ERROR;
    }

    behaviour_status = process_directory(src_path, dest_path);
    if (behaviour_status != 0)
    {
        fprintf(stderr, "%s:%s", "Can't process directory:", dest_path);
        perror("Error");
        return ERROR;
    }
    return 0;
}


const char* get_last_dir(const char* path, char* dir_name) {
    if (path == NULL || *path == '\0') {
        return NULL;
    }
    const char* last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        strncpy(dir_name, path, PATH_MAX);
        return path;
    }
    strncpy(dir_name, last_slash + 1, PATH_MAX);
    return last_slash + 1;

}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_directory>\n", argv[0]);
        return 0;
    }
    char dir_name[PATH_MAX];
    get_last_dir(argv[1], dir_name);
    char reversed_dir[PATH_MAX];


    strcpy(reversed_dir, dir_name);
    reverse_buffer(reversed_dir, strlen(reversed_dir));

    int good_behaviour = mkdir(reversed_dir, DIR_ACCESS);
    if (good_behaviour != 0) {
        fprintf(stderr, "%s:%s\n", "Cant open dir:", reversed_dir);
        perror("Error");
        return 0;
    }

    int reversing_status = process_directory(argv[1], reversed_dir);
    if (reversing_status != 0)
    {
        puts("Errors during reversing, reversed directory is removed");
        rmdir(reversed_dir);
        return 0;
    }
    printf("Directory '%s' copied to '%s' with reversed file names and contents.\n", argv[1], reversed_dir);

    return 0;
}