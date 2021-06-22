#include "iic.h"
 
/*
函数功能：IIC接口初始化
硬件连接：
SDA：PB7
SCL：PB6
*/
void IIC_Init(void)
{
	RCC->APB2ENR|=1<<3;//PB
	GPIOB->CRL&=0x00FFFFFF;
	GPIOB->CRL|=0x33000000;
	GPIOB->ODR|=0x3<<6;
}
 
/*
函数功能：IIC总线起始信号
*/
void IIC_Start(void)
{
		IIC_SDA_OUTMODE(); //初始化SDA为输出模式
		IIC_SDA_HIGH; 		 //数据线拉高
		IIC_SCL_HIGH;     		 //时钟线拉高
		delay_us(4);        //电平保持时间
		IIC_SDA_LOW; 		 //数据线拉低
		delay_us(4);        //电平保持时间
		IIC_SCL_LOW;     		 //时钟线拉低
}
 
 
/*
函数功能：IIC总线停止信号
*/
void IIC_Stop(void)
{
		IIC_SDA_OUTMODE(); //初始化SDA为输出模式
		IIC_SDA_LOW; 		 //数据线拉低
		IIC_SCL_LOW;     		 //时钟线拉低
		delay_us(4);        //电平保持时间
		IIC_SCL_HIGH;     		 //时钟线拉高
		delay_us(4);        //电平保持时间
		IIC_SDA_HIGH; 		 //数据线拉高
}
 
/*
函数功能：获取应答信号
返 回 值：1表示失败，0表示成功
*/
uint8_t IIC_GetACK(void)
{
		uint8_t cnt=0;
		IIC_SDA_INPUTMODE();//初始化SDA为输入模式
		IIC_SDA_HIGH; 		  //数据线上拉
	  	delay_us(2);         //电平保持时间
		IIC_SCL_LOW;     		  //时钟线拉低，告诉从机，主机需要数据
		delay_us(2);         //电平保持时间，等待从机发送数据
	    IIC_SCL_HIGH;     		  //时钟线拉高，告诉从机，主机现在开始读取数据
		while(IIC_SDA_IN)   //等待从机应答信号
		{
				cnt++;
				if(cnt>250)return 1;
		}
		IIC_SCL_LOW;     		  //时钟线拉低，告诉从机，主机需要数据
		return 0;
}
 
/*
函数功能：主机向从机发送应答信号
函数形参：0表示应答，1表示非应答
*/
void IIC_SendACK(uint8_t stat)
{
		IIC_SDA_OUTMODE(); //初始化SDA为输出模式
		IIC_SCL_LOW;     		 //时钟线拉低，告诉从机，主机需要发送数据
		if(stat)IIC_SDA_HIGH; //数据线拉高，发送非应答信号
		else IIC_SDA_LOW; 	 //数据线拉低，发送应答信号
		delay_us(2);            //电平保持时间，等待时钟线稳定
		IIC_SCL_HIGH;     		     //时钟线拉高，告诉从机，主机数据发送完毕
		delay_us(2);            //电平保持时间，等待从机接收数据
		IIC_SCL_LOW;     		  	 //时钟线拉低，告诉从机，主机需要数据
}
 
 
/*
函数功能：IIC发送1个字节数据
函数形参：将要发送的数据
*/
void IIC_WriteOneByteData(uint8_t data)
{
		uint8_t i;
		IIC_SDA_OUTMODE(); //初始化SDA为输出模式
		IIC_SCL_LOW;     		 //时钟线拉低，告诉从机，主机需要发送数据
		for(i=0;i<8;i++)
		{
				if(data&0x80)IIC_SDA_HIGH; //数据线拉高，发送1
				else IIC_SDA_LOW; 	 //数据线拉低，发送0
				IIC_SCL_HIGH;     		     //时钟线拉高，告诉从机，主机数据发送完毕
				delay_us(2);            //电平保持时间，等待从机接收数据
				IIC_SCL_LOW;     		 		 //时钟线拉低，告诉从机，主机需要发送数据
				delay_us(2);            //电平保持时间，等待时钟线稳定
				data<<=1;              //先发高位
		}
}
 
 
/*
函数功能：IIC接收1个字节数据
返 回 值：收到的数据
*/
uint8_t IIC_ReadOneByteData(void)
{
		uint8_t i,data;
		IIC_SDA_INPUTMODE();//初始化SDA为输入模式
	  for(i=0;i<8;i++)
	  {
			 	IIC_SCL_LOW;     		  //时钟线拉低，告诉从机，主机需要数据
				delay_us(2);         //电平保持时间，等待从机发送数据
				IIC_SCL_HIGH;     		  //时钟线拉高，告诉从机，主机现在正在读取数据
				data<<=1;           
				if(IIC_SDA_IN)data|=0x01;
				delay_us(2);         //电平保持时间，等待时钟线稳定
	  }
		IIC_SCL_LOW;     		  		//时钟线拉低，告诉从机，主机需要数据 (必须拉低，否则将会识别为停止信号)
		return data;
}