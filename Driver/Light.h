#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Config.h"


#define LED_L     P07
#define LED_R     P52

typedef enum {

    LIGHT_LEFT,  // 0
    LIGHT_RIGHT, // 1
    LIGHT_ALL,   // 2
    
} Light;

void Light_init();

void Light_on(Light light);

void Light_off(Light light);

#endif