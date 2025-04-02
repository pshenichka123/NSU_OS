#include "forfiles.h"
#define ERROR -1
#define DIR_ACCESS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define FILE_ACCESS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) 

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