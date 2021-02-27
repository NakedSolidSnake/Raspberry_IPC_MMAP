<p align="center">
  <img src="https://www.tutorialsdaddy.com/wp-content/uploads/2016/11/linux-mmap.png">
</p>

# MMAP
## Introdução
## Implementação
## launch_processes.c
```c
/**
 * @file launch_processes.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid_button, pid_led;
    int button_status, led_status;

    pid_button = fork();

    if(pid_button == 0)
    {
        //start button process
        char *args[] = {"./button_process", NULL};
        button_status = execvp(args[0], args);
        printf("Error to start button process, status = %d\n", button_status);
        abort();
    }   

    pid_led = fork();

    if(pid_led == 0)
    {
        //Start led process
        char *args[] = {"./led_process", NULL};
        led_status = execvp(args[0], args);
        printf("Error to start led process, status = %d\n", led_status);
        abort();
    }

    return EXIT_SUCCESS;
}
```
## button_process.c
```c
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

static Button_t button = {
    .gpio.pin = 7,
    .gpio.eMode = eModeInput,
    .ePullMode = ePullModePullUp,
    .eIntEdge = eIntEdgeFalling,
    .cb = NULL};

int main(int argc, char *const argv[])
{

    if (Button_init(&button))
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
        if (!Button_read(&button))
        {
            usleep(_1MS * 40);
            while (!Button_read(&button))
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
```
## led_process.c
```c
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
```
## Conclusão
