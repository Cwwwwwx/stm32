#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f1xx.h"

//操作HAL库函数读取IO口状态
#define KEY_S1	  HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)
#define KEY_S2    HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)
#define KEY_S3    HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)
#define KEY_S4    HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)
//定义按键按下时的键值
#define KEY_S1_PRES 	    1
#define KEY_S2_PRES	    	2
#define KEY_S3_PRES		    3
#define KEY_S4_PRES     	4

uint8_t KEY_Scan(uint8_t mode);

#endif
