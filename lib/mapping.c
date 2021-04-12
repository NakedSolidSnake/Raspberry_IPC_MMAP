#include <mapping.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

void *mapping_file(const char *filename, int file_size)
{
    int fd;
    void *file_memory;

    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    lseek(fd, file_size + 1, SEEK_SET);
    write(fd, "", 1);
    lseek(fd, 0, SEEK_SET);

    /* Create the memory mapping. */
    file_memory = mmap(0, file_size, PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    return file_memory;
}

void mapping_cleanup(void *mapping, int file_size)
{
    munmap(mapping, file_size);
}