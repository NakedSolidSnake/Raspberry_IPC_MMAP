#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <led_interface.h>

#define FILE_LENGTH 0x100

#define FILENAME "/tmp/data.dat"

void *mapping_file(const char *filename)
{
    int fd;
    void *file_memory;

    fd = open(filename, O_RDWR, S_IRUSR | S_IWUSR);

    /* Create the memory mapping. */
    file_memory = mmap(0, FILE_LENGTH, PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);
    close(fd);
    
    return file_memory;
}

void mapping_cleanup(void *file_memory)
{
    munmap(file_memory, FILE_LENGTH);
}

static int getValue(void *file_memory){
    int value = 0;

    sscanf(file_memory, "%d", &value);    

    return value;
}

bool LED_Run(void *object, LED_Interface *led)
{
    int state_curr = 0;
    int state_old = -1;
    void *memory;

    if(led->Init(object) == false)
        return false;

    memory = mapping_file(FILENAME);

    while(true)
    {
        state_curr =  getValue(memory);
        if(state_curr != state_old)
        {
            led->Set(object, state_curr);
            state_old = state_curr;
        }
        usleep(1000);
    }

    mapping_cleanup(memory);

    return false;
}
