#include "Light.h"
#include "GPIO.h"

static void GPIO_config(void) {
    P0_MODE_OUT_PP(GPIO_Pin_7);
    P5_MODE_OUT_PP(GPIO_Pin_2);
}
void Light_init(){

    GPIO_config();
}

// µãÁÁ
void Light_on(Light light){
    switch (light)
    {
    	case LIGHT_LEFT:
            LED_L = 1;
    		break;
    	case LIGHT_RIGHT:
            LED_R = 1;
    		break;
    	case LIGHT_ALL:
            LED_R = LED_L = 1;
    		break;
    	default:
    		break;
    }
    
}

// Ï¨Ãð
void Light_off(Light light){

    switch (light)
    {
    	case LIGHT_LEFT:
            LED_L = 0;
    		break;
    	case LIGHT_RIGHT:
            LED_R = 0;
    		break;
    	case LIGHT_ALL:
            LED_R = LED_L = 0;
    		break;
    	default:
    		break;
    }
    
}