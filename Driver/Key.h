#ifndef __KEY_H__
#define __KEY_H__

#include "Config.h"

#define KEY     P05

void Key_init();

void Key_scan(); // 10ms

extern void Key_on_up();

extern void Key_on_down();

#endif