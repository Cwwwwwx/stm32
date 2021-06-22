#include "bh1750.h"
#include "stdio.h"
uint8_t r_s=0;
float Read_BH1750_Data()
{
    unsigned char t0;
    unsigned char t1;
    unsigned char t;
    unsigned char BUF[8];
    unsigned char i;
    int dis_data;
    float temp1;
    
    
    IIC_Start(); //发送起始信号
    IIC_WriteOneByteData(0x46);
    r_s=IIC_GetACK();//获取应答
    if(r_s)printf("error:1\r\n");
    IIC_WriteOneByteData(0x01);
    r_s=IIC_GetACK();//获取应答
     if(r_s)printf("error:2\r\n");
    IIC_Stop(); //停止信号 
    
    IIC_Start(); //发送起始信号
    IIC_WriteOneByteData(0x46);
    r_s=IIC_GetACK();//获取应答
    if(r_s)printf("error:5\r\n");
    IIC_WriteOneByteData(0x10);
    r_s=IIC_GetACK();//获取应答
    if(r_s)printf("error:6\r\n");
    IIC_Stop(); //停止信号 
    
    HAL_Delay(300); //等待
    
    IIC_Start(); //发送起始信号
    IIC_WriteOneByteData(0x47);
    r_s=IIC_GetACK();//获取应答
    if(r_s)printf("error:7\r\n");
    
    for (i=0; i<2; i++)                      //连续读取2个数据，存储到BUF里面
    {
      BUF[i] = IIC_ReadOneByteData();          //BUF[0]存储高8位，BUF[1]存储低8位
      if (i == 2)
      {
        IIC_SendACK(1);                //最后一个数据需要回NOACK
      }
      else
      {		
        IIC_SendACK(0);                //回应ACK
      }
    }
    
    // t0=IIC_ReadOneByteData(); //接收数据
    // IIC_SendACK(0); //发送应答信号
    // t1=IIC_ReadOneByteData(); //接收数据
    // IIC_SendACK(1); //发送非应答信号
    IIC_Stop(); //停止信号
    dis_data=BUF[0];
    dis_data=(dis_data<<8)+BUF[1]; //2个字节合成数据 
    temp1=dis_data/1.2;//计算光照度
    
    //  t=(((t0<<8)|t1)/1.2);
     return temp1;  
}