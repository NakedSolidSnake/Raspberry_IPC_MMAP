#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <led.h>

#define FILE_LENGTH 0x100

#define FILENAME "/tmp/data.dat"

static int getValue(){
    int value = 0;
    int fd;
    void *file_memory;

    /* Open the file. */
    fd = open(FILENAME, O_RDWR, S_IRUSR | S_IWUSR);

    /* Create the memory mapping. */
    file_memory = mmap(0, FILE_LENGTH, PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);
    close(fd);

    sscanf(file_memory, "%d", &value);

    munmap(file_memory, FILE_LENGTH);

    return value;
}

static LED_t led = {
        .gpio.pin = 0,
        .gpio.eMode = eModeOutput
    };

int main(int argc, char *const argv[])
{
    int state_curr = 0;
    int state_old = -1;

    if(LED_init(&led))
        return EXIT_FAILURE;

    while(1){
        state_curr =  getValue();
        if(state_curr != state_old){
            LED_set(&led, (eState_t)state_curr);
            state_old = state_curr;
        }
        usleep(1000);
    }

    return 0;
}