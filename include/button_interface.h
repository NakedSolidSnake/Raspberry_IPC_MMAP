#ifndef BUTTON_INTERFACE_H_
#define BUTTON_INTERFACE_H_

#include <stdbool.h>

/**
 * @brief 
 * 
 */
typedef struct 
{
    bool (*Init)(void *object);
    bool (*Read)(void *object);
    
} Button_Interface;

/**
 * @brief 
 * 
 * @param object 
 * @param argv 
 * @param button 
 * @return true 
 * @return false 
 */
bool Button_Run(void *object, Button_Interface *button);

#endif /* BUTTON_INTERFACE_H_ */
