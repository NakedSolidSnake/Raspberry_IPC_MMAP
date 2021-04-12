#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <button_interface.h>

#define FILE_LENGTH 0x100
#define _1ms    1000

#define FILENAME "/tmp/data.dat"

static int toogle()
{
    static int state = 0;
    state ^= 0x01;
    return state;
}

static void setValue(void *file_memory)
{  
    /* Write a random integer to memory-mapped area. */
    sprintf((char *)file_memory, "% d\n", toogle());
}

void *b_mapping_file(const char *filename)
{
    int fd;
    void *file_memory;

    fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    lseek(fd, FILE_LENGTH + 1, SEEK_SET);
    write(fd, "", 1);
    lseek(fd, 0, SEEK_SET);

    /* Create the memory mapping. */
    file_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    return file_memory;
}

void b_mapping_cleanup(void *file_memory)
{
    munmap(file_memory, FILE_LENGTH);
}


bool Button_Run(void *object, Button_Interface *button)
{
    void *memory;    

    if (button->Init(object) == false)
        return false;

    memory = b_mapping_file(FILENAME);

    while (true)
    {
        while (true)
        {
            if (!button->Read(object))
            {
                usleep(_1ms * 100);
                break;
            }
            else
            {
                usleep(_1ms);
            }
        }

        setValue(memory);
    }

    b_mapping_cleanup(memory);

    return false;
}
