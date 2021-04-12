#include <led_interface.h>
#include <mapping.h>
#include <stdio.h>
#include <unistd.h>

#define FILE_LENGTH 0x100

#define FILENAME "/tmp/data.dat"

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

    memory = mapping_file(FILENAME, FILE_LENGTH);

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

    mapping_cleanup(memory, FILE_LENGTH);

    return false;
}
