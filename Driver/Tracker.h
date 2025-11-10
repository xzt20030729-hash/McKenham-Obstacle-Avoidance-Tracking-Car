#ifndef __TRACKER_H__
#define __TRACKER_H__

#include "Config.h"

#define TRACK_0  P00
#define TRACK_1  P01
#define TRACK_2  P02
#define TRACK_3  P03
#define TRACK_4  P04

void Tracker_init();

// 扫描所有灯的状态
void Tracker_scan(u8 states[]);

// 获取黑线位置信息(旋转角度信息)
int Tracker_get_position();

#endif