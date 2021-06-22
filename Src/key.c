#include "key.h"

/*******************************************
����ԭ�ͣ�uint8_t KEY_Scan(uint8_t mode)
�� �� ֵ��mode = 0 ���ΰ��°���
					mode = 1 �������°���
�� �� ֵ��0 û�а�������
					1	KEY_RIGHT����
					2	KEY_DOWN����
					3	KEY_LEFT����
					4	KEY_UP����
*******************************************/
uint8_t KEY_Scan(uint8_t mode)
{
	static uint8_t key = 1;		//�����ɿ���־
	if(mode == 1){						//֧������
		key = 1;
	}
	
	if(key&&(KEY_S4==1||KEY_S1==0||KEY_S2==0||KEY_S3==0)){
		HAL_Delay(100);					//��������
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

