#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "Config.h"

// TRIG ´¥·¢²â¾à
#define TRIG    P47

// ECHO Êä³ö»ØÏì
#define ECHO    P33


void Ultrasonic_init();


char Ultrasonic_get_distance(float* distance);
#endif