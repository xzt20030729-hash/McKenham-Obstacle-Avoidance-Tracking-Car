#include "Key.h"
#include "GPIO.h"


void Key_init(){
    P0_MODE_IO_PU(GPIO_Pin_5);
}


void Key_scan(){
    
    static u8 pre_state = 1; // 静态局部变量
    // 如果状态发生变化
    if(KEY != pre_state){
        
        if(KEY) { // 抬起
            Key_on_up();
        }else { // 按下
            Key_on_down();
        }
        
        // 记录状态
        pre_state = KEY;
    }
    
}