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
    
    
    IIC_Start(); //������ʼ�ź�
    IIC_WriteOneByteData(0x46);
    r_s=IIC_GetACK();//��ȡӦ��
    if(r_s)printf("error:1\r\n");
    IIC_WriteOneByteData(0x01);
    r_s=IIC_GetACK();//��ȡӦ��
     if(r_s)printf("error:2\r\n");
    IIC_Stop(); //ֹͣ�ź� 
    
    IIC_Start(); //������ʼ�ź�
    IIC_WriteOneByteData(0x46);
    r_s=IIC_GetACK();//��ȡӦ��
    if(r_s)printf("error:5\r\n");
    IIC_WriteOneByteData(0x10);
    r_s=IIC_GetACK();//��ȡӦ��
    if(r_s)printf("error:6\r\n");
    IIC_Stop(); //ֹͣ�ź� 
    
    HAL_Delay(300); //�ȴ�
    
    IIC_Start(); //������ʼ�ź�
    IIC_WriteOneByteData(0x47);
    r_s=IIC_GetACK();//��ȡӦ��
    if(r_s)printf("error:7\r\n");
    
    for (i=0; i<2; i++)                      //������ȡ2�����ݣ��洢��BUF����
    {
      BUF[i] = IIC_ReadOneByteData();          //BUF[0]�洢��8λ��BUF[1]�洢��8λ
      if (i == 2)
      {
        IIC_SendACK(1);                //���һ��������Ҫ��NOACK
      }
      else
      {		
        IIC_SendACK(0);                //��ӦACK
      }
    }
    
    // t0=IIC_ReadOneByteData(); //��������
    // IIC_SendACK(0); //����Ӧ���ź�
    // t1=IIC_ReadOneByteData(); //��������
    // IIC_SendACK(1); //���ͷ�Ӧ���ź�
    IIC_Stop(); //ֹͣ�ź�
    dis_data=BUF[0];
    dis_data=(dis_data<<8)+BUF[1]; //2���ֽںϳ����� 
    temp1=dis_data/1.2;//������ն�
    
    //  t=(((t0<<8)|t1)/1.2);
     return temp1;  
}