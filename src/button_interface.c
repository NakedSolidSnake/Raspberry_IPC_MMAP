#include <mapping.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
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

static void wait_press(void *object, Button_Interface *button)
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
}

static void setValue(void *file_memory)
{  
    /* Write a random integer to memory-mapped area. */
    sprintf((char *)file_memory, "%d\n", toogle());
}

bool Button_Run(void *object, Button_Interface *button)
{
    void *memory;    

    if (button->Init(object) == false)
        return false;

    memory = mapping_file(FILENAME, FILE_LENGTH);

    while (true)
    {
        wait_press(object, button);
        setValue(memory);
    }

    mapping_cleanup(memory, FILE_LENGTH);

    return false;
}
