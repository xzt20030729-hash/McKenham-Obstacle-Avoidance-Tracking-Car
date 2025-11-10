#include "Buzzer.h"
#include "STC8H_PWM.h"
#include "GPIO.h"
#include "NVIC.h"
#include "Switch.h"

//			 C	 D     E 	F	 G	  A	   B	 C`
// u16 hz[] = {523, 587, 659, 698, 784, 880, 988, 1047};
//			 C`	   D`     E`   F`	  G`	A`	  B`    C``
//u16 hz[] = {1047, 1175, 1319, 1397, 1568, 1760, 1976, 2093};

u16 code FREQS[] = {
	523 * 1, 587 * 1, 659 * 1, 698 * 1, 784 * 1, 880 * 1, 988 * 1, 
	523 * 2, 587 * 2, 659 * 2, 698 * 2, 784 * 2, 880 * 2, 988 * 2, 
	523 * 4, 587 * 4, 659 * 4, 698 * 4, 784 * 4, 880 * 4, 988 * 4, 
	523 * 8, 587 * 8, 659 * 8, 698 * 8, 784 * 8, 880 * 8, 988 * 8, 
};

static u8 freqs_cnt = sizeof(FREQS) / sizeof(FREQS[0]);

static void GPIO_config(void) {
    GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
    GPIO_InitStructure.Pin  = BUZZER_GPIO_PIN;		//指定要初始化的IO,
    GPIO_InitStructure.Mode = GPIO_OUT_PP;	    //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(BUZZER_GPIO_PORT, &GPIO_InitStructure); //初始化
}

void Buzzer_init(){
    EAXSFR();		/* 扩展寄存器访问使能 */
    GPIO_config();
    
    BUZZER_PIN = 0;
}

void Buzzer_play(u16 hz_val) {
    u16 period = MAIN_Fosc / hz_val;

    PWMx_InitDefine		PWMx_InitStructure;

    // 配置PWM8
    PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	    //模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty   	 	= (u16)(period * 0.5f);	//PWM占空比时间, 0~Period
    PWMx_InitStructure.PWM_EnoSelect    = ENO8P;			    //输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM8, &PWMx_InitStructure);			    //初始化PWM,  PWMA,PWMB

    // 配置PWMB
    PWMx_InitStructure.PWM_Period   = period - 1;			//周期时间,   0~65535
    PWMx_InitStructure.PWM_DeadTime = 0;					//死区发生器设置, 0~255
    PWMx_InitStructure.PWM_MainOutEnable= ENABLE;			//主输出使能, ENABLE,DISABLE
    PWMx_InitStructure.PWM_CEN_Enable   = ENABLE;			//使能计数器, ENABLE,DISABLE
    PWM_Configuration(PWMB, &PWMx_InitStructure);			//初始化PWM通用寄存器,  PWMA,PWMB

    // 切换PWM通道
	PWM8_SW(PWM8_SW_P34);					//PWM8_SW_P23,PWM8_SW_P34,PWM8_SW_P03,PWM8_SW_P77

    // 初始化PWMB的中断
    NVIC_PWM_Init(PWMB,DISABLE,Priority_0);
}

void Buzzer_beep(u16 idx){ // 0
    if(idx == 0){ // 不发音
        Buzzer_stop();
        return;
    }
    
    // 避免溢出
    if(idx > freqs_cnt) idx = freqs_cnt;
    
    Buzzer_play(FREQS[idx - 1]);
}

/**********************************************************
 * @brief 蜂鸣器报警
 * @param count    响声次数
 * @param duration 每次时长ms
 * @param dir      升降调方向 +1, -1, 0
 **********************************************************/
void Buzzer_alarm(u8 count, u16 duration, int8 dir){
    u8 note = 8;
    
    while(count--){
        // 播放指定音符
        Buzzer_beep(note);
        os_wait2(K_TMO, duration / 5); // 20 * 5ms = 100ms
        
        // 暂停播放
        Buzzer_stop();    
        os_wait2(K_TMO, 10); // 10 * 5ms = 50ms
        
        // 根据音调变化方向更新音符
        note += dir;
    }
    
    Buzzer_stop();    
}

void Buzzer_stop(){
    PWMx_InitDefine		PWMx_InitStructure;
    // 配置PWM8
    PWMx_InitStructure.PWM_EnoSelect    = 0;		//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM8, &PWMx_InitStructure);	 //初始化PWM,  PWMA,PWMB

//    PWMx_InitDefine		PWMx_InitStructure;
//    PWMx_InitStructure.PWM_MainOutEnable= DISABLE;			//主输出使能, ENABLE,DISABLE
//    PWMx_InitStructure.PWM_CEN_Enable   = DISABLE;			//使能计数器, ENABLE,DISABLE
//    PWM_Configuration(PWMB, &PWMx_InitStructure);			//初始化PWM通用寄存器,  PWMA,PWMB
}
