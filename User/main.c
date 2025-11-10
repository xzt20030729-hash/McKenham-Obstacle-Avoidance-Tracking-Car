#include "config.h"
#include "delay.h"

#include "GPIO.h"
#include "UART.h"
#include "Switch.h"
#include "NVIC.h"
#include "STC8H_PWM.h"

#include "Light.h"
#include "Key.h"
#include "Battery.h"
#include "Buzzer.h"
#include "Ultrasonic.h"
#include "Motors.h"
#include "Tracker.h"

/**********************
需求: 测试4个电机的正反转

任务1: 按键扫描

按下按键: 切换前进, 后退, 左平移, 右平移, 掉头
抬起按键: 停止

驱动集成:
1. 超声测距模块, 如果距离小于20cm (连续3次), 停车 (蜂鸣器响起3次)
2. 电池电压检测, 如果电压小于10.5V (连续5次), 停车 (蜂鸣器响起5次)

***********************/
static void GPIO_config(void) {
    
}

void UART_config(void) {
  // >>> 记得添加 NVIC.c, UART.c, UART_Isr.c <<<
  COMx_InitDefine		COMx_InitStructure;					//结构定义
  COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
  COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
  COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
  COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
  COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
  UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4

  NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
  UART1_SW(UART1_SW_P30_P31);		// 引脚选择, UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

void sys_init() {
    EAXSFR();// 外部扩展寄存器使能

    GPIO_config();
    UART_config();
//    PWM_config();

    Light_init();
    Key_init();
    Battery_init();
    Buzzer_init();
    Motors_init();
    Tracker_init();
    
    EA = 1;
}

// 这里函数名可随意, 不要使用start, 会和I2C.h里的Start冲突
void task_main() _task_ 0 {
    // 外设初始化
    sys_init();
    // 创建任务 1
    os_create_task(1);
    // 创建任务 2
    os_create_task(2);
    // 结束任务 0
    os_delete_task(0);
}

u8 move_state = 0;
void Key_on_up(){
    Light_off(LIGHT_ALL);
    printf("Key up\n");
//    Motors_stop();
    
}

// F: forward前进
// B: backward后退
//u8 is_forward = 1;  
/****************************
duty:     0 ------------------------ 50 --------------------- 100
speed: B_Max                          0                       F_Max

50 -> 60 -> .. -> 100 -> 90 -> 80 .... -> 0 -> 10 -> 20 ...-> 50
****************************/

float duty = 50.0f;
int direction_step = 10;

u8 is_tracking = FALSE; // 循迹中

void Key_on_down(){
    float Vbat = 0;    
    Light_on(LIGHT_ALL);
    printf("Key down\n");
        
    // 取反
    is_tracking = !is_tracking;
//    
//    // 0:前进, 1:后退
//    switch (move_state)
//    {
//        case 0: Motors_forward(30); break;       // 前进
//        case 1: Motors_backward(30); break;     // 后退
//        case 2: Motors_move_left(30, 1); break;    // 左平移
//        case 3: Motors_move_right(30, -1); break;   // 右平移
//        case 4: Motors_around(30, 1); break;    // 顺时针
//        case 5: Motors_around(30, 0); break;    // 逆时针
//        default: Motors_stop();break;
//    } 
//    move_state++;
//    if(move_state > 6) move_state = 0;

//    // 运行1秒，停车
//    os_wait2(K_TMO, 200); // 200 * 5ms = 1000ms

//    // 停车
//    Motors_stop();

}

// 任务1: 
void task_1() _task_ 1 {
    
    printf("task1\n");
    
    Light_off(LIGHT_ALL);
        
    while(1) {    
        
        Key_scan();        
        // 间隔10ms
        os_wait2(K_TMO, 2); // 10ms = 5ms * 2
    }

}


// 任务2: 
void task_tracking() _task_ 2 {
    u8 states[5];
    u8 i = 0;
    
    int pos = 0, last_pos = -10000; // last_pos初始化为一个pos不可能出现的数字, 保证第一次能运行
    
    while(1){    
        // 每隔100ms读取一次LED灯状态
//        Tracker_scan(states);
//        printf("->");
//        for(i = 0; i < 5; i++){
//            printf("%d ", (int)states[i]);
//        }
//        printf("\n");
        // 获取当前循迹的目标运行角度(方向) 负数向左, 正数向右
        pos = Tracker_get_position();        
//        printf("pos -> %d\n", pos);
        
        // 先判断是否需要运行循迹模块
        if(is_tracking == FALSE){           
            // 保证不循迹时, 车可以停下来
            Motors_stop(); 
            // 减慢默认循环速度
            os_wait2(K_TMO, 100); // 500ms            
            printf("停止中, 按下按钮开始巡线\n");            
            // 跳过本次循环
            continue;
        }
        
        // 再判断是否需要保持之前的运动方式
        if(pos == last_pos){        
            os_wait2(K_TMO, 2); // 2 * 5ms = 10ms                  
            // 跳过本次循环, 保持之前的运动方式, 避免频繁操作寄存器
            continue;
        }
        
        // 记录最后一次黑线位置
        last_pos = pos;
        
        if(pos < 0){ 
            Motors_around(30, 0); // 向左转(逆时针)
            printf("左转: %d\n", pos);
        }else if(pos > 0){
            Motors_around(30, 1); // 向右转(顺时针)            
            printf("右转: %d\n", pos);            
        }else {
            Motors_forward(30);
            printf("直行: %d\n", pos);            
        }
        
//        os_wait2(K_TMO, 20); // 20 * 5ms = 100ms
        
        os_wait2(K_TMO, 2); // 2 * 5ms = 10ms 加快传感器感知频率
        
    }
}