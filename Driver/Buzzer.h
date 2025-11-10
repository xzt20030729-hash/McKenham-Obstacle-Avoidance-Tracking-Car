#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "Config.h"

#define BUZZER_GPIO_PORT    GPIO_P3
#define BUZZER_GPIO_PIN     GPIO_Pin_4

#define BUZZER_PIN          P34


void Buzzer_init();

// 按照指定频率播放
void Buzzer_play(u16 hz_val);

// 指定音调音符播放 12345678
void Buzzer_beep(u16 idx);

// 蜂鸣器报警
void Buzzer_alarm(u8 count, u16 duration, int8 dir);

// 停车
void Buzzer_stop();

#endif