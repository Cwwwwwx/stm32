#ifndef _IIC_H
#define _IIC_H
#include "main.h"
#include "tim.h"
#define IIC_SDA_OUTMODE() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x30000000;}
#define IIC_SDA_INPUTMODE() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define IIC_SDA_HIGH HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,1)   //数据线输出
#define IIC_SDA_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,0)   //数据线输出
#define IIC_SDA_IN HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)  //数据线输入
#define IIC_SCL_HIGH HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,1)  //时钟线
#define IIC_SCL_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,0)  //时钟线
 
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_GetACK(void);
void IIC_SendACK(uint8_t stat);
void IIC_WriteOneByteData(uint8_t data);
uint8_t IIC_ReadOneByteData(void);
#endif
