#include "Battery.h"
#include "ADC.h"
#include "NVIC.h"
#include "GPIO.h"

static void GPIO_config(void) {
    
    // P13 高阻输入(或准双向)
    P1_MODE_IN_HIZ(GPIO_Pin_3);
    
}
/******************* AD配置函数 *******************/
static void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//结构定义

	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC 模拟信号采样时间控制, 0~31（注意： SMPDUTY 一定不能设置小于 10）
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC 通道选择时间控制 0(默认),1
	ADC_InitStructure.ADC_CsHold    = 1;		//ADC 通道选择保持时间控制 0,1(默认),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X1T;		//设置 ADC 工作时钟频率	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC结果调整,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_Inilize(&ADC_InitStructure);		//初始化
	ADC_PowerControl(ENABLE);				//ADC电源开关, ENABLE或DISABLE
	NVIC_ADC_Init(DISABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}


void Battery_init(){
    GPIO_config();
    ADC_config();
}

float Battery_get_voltage(){
    u16 adc_value = 0;
    float Vadc = 0, Vbat = 0;
    adc_value = Get_ADCResult(ADC_CH3); // chn: ADC_CH0, ADC_CH1...ADC_CH15
    // 将ADC数值转成电压 Vadc / 2.5V = adc_value / 4096
    Vadc = adc_value * 2.5f / 4096;
    
    // 根据电阻比例计算电池电压
//    Vbat = Vadc * (51 + 10) / 10;
    Vbat = Vadc * 6.1f;
    
    // 2.5V / 10 = Vbat / (51 + 10)
    // Vbat = 2.5 * 61 / 10 = 15.25V
    // Vbat = 2.5 * 6.1 = 15.25V
    
    return Vbat;
}