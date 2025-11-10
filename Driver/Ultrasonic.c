#include "Ultrasonic.h"
#include "GPIO.h"

static void GPIO_config(void) {
    
    // TRIG 推挽输出
    P4_MODE_OUT_PP(GPIO_Pin_7);
    
    // ECHO 高阻输入    
    P3_MODE_IN_HIZ(GPIO_Pin_3);    
    
    // 把引脚拉低
    TRIG = 0;
}

void Ultrasonic_init(){

    GPIO_config();
}

void Delay10us(void)	//@24.000MHz
{
	unsigned char data i;

	i = 78;
	while (--i);
    
}

/************************

通过超声波读取距离

distance: 成功时, 自动填充距离数据, 单位cm

11.18ms * 340 m/s
11.18ms * 34000/1000 cm/ms
11.18ms * 34 cm/ms / 2

返回: 0成功SUCCESS,  -1失败FAIL 其他也是失败
*************************/
char Ultrasonic_get_distance(float* distance){ // cm
    u16 cnt_10us = 0;
    
    // 1. 拉高TRIG引脚10us以上再拉低(脉冲)
    TRIG = 1;
    Delay10us();
    Delay10us();
    TRIG = 0;
    
    // 2. 等待ECHO被拉高
    while(ECHO == 0 && cnt_10us <= 300){ // 
        Delay10us(); // 10us
        cnt_10us++; // 300 -> 300 * 10us = 3000us = 3ms
    }    
    // 一定时间内没拉高, 返回失败
    if(cnt_10us >= 300){
        printf("等待ECHO拉高超时: %d us, 退出!\n", (int)(cnt_10us * 10) );        
        return FAIL;
    }
    
    // 3. 记录ECHO从上升沿到下降沿的间隔时间
    cnt_10us = 0;
    while(ECHO == 1){
        Delay10us(); // 10us
        cnt_10us++;
    }
    
    // dis(cm) = ((cnt_10us * 10)us * 340m/s) * 0.5f;
    // dis(cm) = ((cnt_10us * 0.01)ms * (34000/1000) cm/ms) * 0.5f;
    // dis(cm) = ((cnt_10us * 0.01)ms * 34 cm/ms) * 0.5f;
    *distance = ((cnt_10us * 0.01f) * 34) * 0.5f;
//    *distance = cnt_10us * 0.17f;
    
    if(*distance < 2.0){ // 距离太短
        return 1; // 不保证准确,太近
    }else if(*distance > 400.0){ // 距离太远
        return 2; // 不保证准确,太远
    }
    
    return SUCCESS;
}