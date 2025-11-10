#include "Motors.h"
#include "GPIO.h"
#include "Switch.h"
#include "NVIC.h"
#include	"STC8H_PWM.h"

// Left Front  ZuoQian
#define LF_P    P14
#define LF_N    P15

static void GPIO_config(void) {
    // Motor P14,P15 推挽输出
    P1_MODE_OUT_PP(GPIO_Pin_4 | GPIO_Pin_5);
    // Motor P16,P17 推挽输出
    P1_MODE_OUT_PP(GPIO_Pin_6 | GPIO_Pin_7);
    // Motor P20,P21 推挽输出
    P2_MODE_OUT_PP(GPIO_Pin_0 | GPIO_Pin_1);
    // Motor P22,P23 推挽输出
    P2_MODE_OUT_PP(GPIO_Pin_2 | GPIO_Pin_3);
    
    // 都拉低或默认拉高都可以
    P1 &= ~GPIO_Pin_HIGH; // 把 P14,P15,P16,P17 拉低
    P2 &= ~GPIO_Pin_LOW; //  把 P20,P21,P22,P23 拉低
}

#define PERIOD (MAIN_Fosc / 1000)  // 1000Hz

/**********************
将范围在[-100, 100]的速度值转成对应的占空比[0, 100]

speed: 后退                         停止                      前进

speed: -100 ------------------------  0 --------------------- 100
duty:     0 ------------------------ 50 --------------------- 100
************************/
float speed2duty(float speed){
    // [-100, 100] / 2  ==> [-50, 50]
    // [-50, 50] + 50   ==> [-100, 100]
    return speed / 2 + 50;
}

/****************
速度范围: [-100, 0, 100]
占空比范围: [0, 50, 100]
PWM_Duty: [0, PERIOD]   
******************/
void	PWM_config(MotorCfg cfg)
{
    PWMx_InitDefine		PWMx_InitStructure;
    // 将所有速度值转成PWM_Duty占空比
    float LF_duty = speed2duty(cfg.LF_speed);
    float RF_duty = speed2duty(cfg.RF_speed);
    float LB_duty = speed2duty(cfg.LB_speed);
    float RB_duty = speed2duty(cfg.RB_speed);
    
    // 如果指定轮子的speed不是0, 才选择该输出通道    
    u8 is_LF_enable = (cfg.LF_speed != 0); // 如果速度非0, 才启用对应的输出通道
    u8 is_RF_enable = (cfg.RF_speed != 0); // 如果速度非0, 才启用对应的输出通道
    u8 is_LB_enable = (cfg.LB_speed != 0); // 如果速度非0, 才启用对应的输出通道
    u8 is_RB_enable = (cfg.RB_speed != 0); // 如果速度非0, 才启用对应的输出通道
    
	// 配置PWM1 左后轮 LB
	PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	PWMx_InitStructure.PWM_Duty    		= PERIOD * LB_duty / 100;	//PWM占空比时间, 0~Period
	PWMx_InitStructure.PWM_EnoSelect	= is_LB_enable ? (ENO1P | ENO1N) : 0x00;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
	PWM_Configuration(PWM1, &PWMx_InitStructure);			//初始化PWM
	// 配置PWM2 右后轮 RB
	PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE2;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	PWMx_InitStructure.PWM_Duty    		= PERIOD * RB_duty / 100;	//PWM占空比时间, 0~Period
	PWMx_InitStructure.PWM_EnoSelect    = is_RB_enable ? (ENO2P | ENO2N) : 0x00;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
	PWM_Configuration(PWM2, &PWMx_InitStructure);			//初始化PWM
	// 配置PWM3 左前轮 LF    
	PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	PWMx_InitStructure.PWM_Duty    		= PERIOD * LF_duty / 100;	//PWM占空比时间, 0~Period
	PWMx_InitStructure.PWM_EnoSelect    = is_LF_enable ? (ENO3P | ENO3N) : 0x00;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
	PWM_Configuration(PWM3, &PWMx_InitStructure);	
	// 配置PWM4 右前轮 RF
    PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE2;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    		= PERIOD * RF_duty / 100;	//PWM占空比时间, 0~Period
    PWMx_InitStructure.PWM_EnoSelect    = is_RF_enable ? (ENO4P | ENO4N) : 0x00;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM4, &PWMx_InitStructure);

	// 配置PWMA
    PWMx_InitStructure.PWM_Period   = PERIOD - 1;			//周期时间,   0~65535
    PWMx_InitStructure.PWM_DeadTime = 10;					//死区发生器设置, 0~255
    PWMx_InitStructure.PWM_MainOutEnable= ENABLE;			//主输出使能, ENABLE,DISABLE
    PWMx_InitStructure.PWM_CEN_Enable   = ENABLE;			//使能计数器, ENABLE,DISABLE
    PWM_Configuration(PWMA, &PWMx_InitStructure);			//初始化PWM通用寄存器,  PWMA,PWMB

	// 切换PWM通道
	PWM1_SW(PWM1_SW_P20_P21);			//PWM1_SW_P10_P11,PWM1_SW_P20_P21,PWM1_SW_P60_P61
	PWM2_SW(PWM2_SW_P22_P23);			//PWM2_SW_P12_P13,PWM2_SW_P22_P23,PWM2_SW_P62_P63
	PWM3_SW(PWM3_SW_P14_P15);			//PWM3_SW_P14_P15,PWM3_SW_P24_P25,PWM3_SW_P64_P65
    PWM4_SW(PWM4_SW_P16_P17);			//PWM4_SW_P16_P17,PWM4_SW_P26_P27,PWM4_SW_P66_P67,PWM4_SW_P34_P33

	// 初始化PWMA的中断
    NVIC_PWM_Init(PWMA,DISABLE,Priority_0);
}

void Motors_init(){
    GPIO_config();
}

// [0, 100] -> [50, 100]
void Motors_forward(float speed){
    MotorCfg cfg;
    cfg.LF_speed = speed;
    cfg.RF_speed = speed;
    cfg.LB_speed = speed;
    cfg.RB_speed = speed;
    PWM_config(cfg);
}

// [0, 100] -> [50, 0]
void Motors_backward(float speed){
    MotorCfg cfg;
    cfg.LF_speed = -speed;
    cfg.RF_speed = -speed;
    cfg.LB_speed = -speed;
    cfg.RB_speed = -speed;
    PWM_config(cfg);
}

// 左平移
// dir: 0左平移, 1左前, -1左后
void Motors_move_left(float speed, int8 dir){    
    MotorCfg cfg = {0, 0, 0, 0};
    
//    if(dir == 1){ // 左前移动
//        cfg.RF_speed = speed; 
//        cfg.LB_speed = speed; 
//    } else if(dir == -1){ // 左后移动
//        cfg.LF_speed = -speed;
//        cfg.RB_speed = -speed;        
//    } else { // 左平移
//        cfg.RF_speed = speed; 
//        cfg.LB_speed = speed; 
//        cfg.LF_speed = -speed;
//        cfg.RB_speed = -speed;        
//    }
    
    if(dir == 1 || dir == 0){ // 左前移动
        cfg.RF_speed = speed; 
        cfg.LB_speed = speed; 
    } 

    if(dir == -1 || dir == 0){ // 左后移动
        cfg.LF_speed = -speed;
        cfg.RB_speed = -speed;        
    }
    
    PWM_config(cfg);
}
// 右平移
// dir: 0右平移, 1右前, -1右后
void Motors_move_right(float speed, int8 dir){

    MotorCfg cfg = {0, 0, 0, 0};
    
    if(dir == 1){ // 右前移动
        cfg.LF_speed = speed;
        cfg.RB_speed = speed;
    } else if(dir == -1){ // 右后移动
        cfg.RF_speed = -speed;
        cfg.LB_speed = -speed;
    } else { // 右平移
        cfg.LF_speed = speed;
        cfg.RB_speed = speed;
        cfg.RF_speed = -speed;
        cfg.LB_speed = -speed;      
    }
    PWM_config(cfg);
}

// 原地转 dir:1 顺时针,  dir: 0逆时针
void Motors_around(float speed, char dir){
    MotorCfg cfg;
    if(dir){ // 顺时针    
        cfg.LF_speed = speed;
        cfg.RF_speed = -speed;
        cfg.LB_speed = speed;
        cfg.RB_speed = -speed;
    }else {  // 逆时针        
        cfg.LF_speed = -speed;
        cfg.RF_speed = speed;
        cfg.LB_speed = -speed;
        cfg.RB_speed = speed;
    }
    PWM_config(cfg);
}

void Motors_stop(){
    MotorCfg cfg;
    cfg.LF_speed = 0;
    cfg.RF_speed = 0;
    cfg.LB_speed = 0;
    cfg.RB_speed = 0;
    PWM_config(cfg);
}
