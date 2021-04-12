#ifndef LED_INTERFACE_H_
#define LED_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 
 * 
 */
typedef struct 
{
    bool (*Init)(void *object);
    bool (*Set)(void *object, uint8_t state);
} LED_Interface;

/**
 * @brief 
 * 
 * @param object 
 * @param argv 
 * @param led 
 * @return true 
 * @return false 
 */
bool LED_Run(void *object, LED_Interface *led);

#endif /* LED_INTERFACE_H_ */
