#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <button.h>

#define FILE_LENGTH 0x100
#define _1MS 1000

#define FILENAME "/tmp/data.dat"

static int toogle()
{
    static int state = 0;
    state ^= 0x01;
    return state;
}

static void setValue(void)
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

    /* Write a random integer to memory-mapped area. */
    sprintf((char *)file_memory, "% d\n", toogle());

    /* Release the memory (unnecessary because the program exits). */
    munmap(file_memory, FILE_LENGTH);
}

static void wait_command(void);

static Button_t button7 = {
    .gpio.pin = 7,
    .gpio.eMode = eModeInput,
    .ePullMode = ePullModePullUp,
    .eIntEdge = eIntEdgeFalling,
    .cb = NULL};

int main(int argc, char *const argv[])
{

    if (Button_init(&button7))
        return EXIT_FAILURE;

    while (1)
    {
        wait_command();
        setValue();
    }

    return 0;
}

static void wait_command(void)
{
    while (1)
    {
        if (!Button_read(&button7))
        {
            usleep(_1MS * 40);
            while (!Button_read(&button7))
                ;
            usleep(_1MS * 40);
            break;
        }
        else
        {
            usleep(_1MS);
        }
    }    
}