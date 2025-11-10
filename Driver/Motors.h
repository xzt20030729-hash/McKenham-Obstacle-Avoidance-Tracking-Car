#ifndef __MOTORS_H__
#define __MOTORS_H__

#include "Config.h"

// 保存轮子的速度
typedef struct {

    // -100 ------------ 0 ------------ 100
    float LF_speed; // 左前轮
    float RF_speed; // 右前轮
    float LB_speed; // 左后轮
    float RB_speed; // 右后轮
    
} MotorCfg; // Motor Config

void Motors_init();

void Motors_forward(float speed);

void Motors_backward(float speed);

// 左平移
void Motors_move_left(float speed, int8 dir);
// 右平移
void Motors_move_right(float speed, int8 dir);

// 原地转 dir:1 顺,  dir:-1逆时针
void Motors_around(float speed, char dir);

void Motors_stop();

#endif