#include "Tracker.h"
#include "GPIO.h"

void Tracker_init(){    
    // 初始化引脚
    P0_MODE_IO_PU(GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
}


// 扫描所有灯的状态, 把所有状态存到数组
// 亮的灯: 0低电平, 地面(光敏电阻收到反射的光)
// 灭的灯: 1高电平, 黑线(光敏电阻没有收到光)
void Tracker_scan(u8 states[]){
    
    states[0] = TRACK_0;
    states[1] = TRACK_1;
    states[2] = TRACK_2;
    states[3] = TRACK_3;
    states[4] = TRACK_4;
    
}
// 读取黑线位置

// [-64, -32, 0, 32, 64]
int Tracker_get_position(){
    static int last_pos = 0;
    int sum = 0, cnt = 0, avg_pos = 0;
    
    if(TRACK_0){
        sum += -64;
        cnt++;
    }
    if(TRACK_1){
        sum += -32;
        cnt++;
    }
    if(TRACK_2){
        sum += 0;
        cnt++;
    }
    if(TRACK_3){
        sum += 32;
        cnt++;
    }
    if(TRACK_4){
        sum += 64;
        cnt++;
    }
    // 没压到黑线 
    if(cnt == 0){
        // 状态丢失, 返回最后一次状态
        return last_pos;
    }
    // 压到线
    avg_pos = sum / cnt;
    
    // 记录当前值, 作为上一次状态, 用于下一次没压线的状态
    last_pos = avg_pos;
    
    return avg_pos;
}