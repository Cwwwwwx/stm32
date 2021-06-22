#include "key.h"

/*******************************************
函数原型：uint8_t KEY_Scan(uint8_t mode)
传 入 值：mode = 0 单次按下按键
					mode = 1 连续按下按键
返 回 值：0 没有按键按下
					1	KEY_RIGHT按下
					2	KEY_DOWN按下
					3	KEY_LEFT按下
					4	KEY_UP按下
*******************************************/
uint8_t KEY_Scan(uint8_t mode)
{
	static uint8_t key = 1;		//按键松开标志
	if(mode == 1){						//支持连按
		key = 1;
	}
	
	if(key&&(KEY_S4==1||KEY_S1==0||KEY_S2==0||KEY_S3==0)){
		HAL_Delay(100);					//按键消抖
		key = 0;
		if(KEY_S4==1)			return KEY_S4_PRES;
		else if(KEY_S1==0)		return KEY_S1_PRES; 
		else if(KEY_S2==0)		return KEY_S2_PRES; 
		else if(KEY_S3==0)		return KEY_S3_PRES; 
	}
	else if(KEY_S4==0&&KEY_S1==1&&KEY_S2==1&&KEY_S3==1){
		key = 1;
	}
	
	return 0;
}

