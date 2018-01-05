#include "dht12.h" 
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//24CXX���� ����(�ʺ�24C01~24C16,24C32~256δ��������!�д���֤!)		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

static uint8_t dat[5];//��������

//uint16_t humid, temper;
float humid, temper;
  
//��ʼ��IIC�ӿ�
void DHT12_Init(void)  
{  
    IIC_Init();  
}  
  
static void DHT12_ReadByte(void)  
{                   
    uint8_t i;                                                                                 
	IIC_Start( ); //�����������߿�ʼ�ź� 
  
    IIC_Send_Byte(0xB8);          
    IIC_Wait_Ack( );  
  
    IIC_Send_Byte(0x00);
    IIC_Wait_Ack( );              
  
    IIC_Start( );//�������ģʽ������0xB8������  
    IIC_Send_Byte(0xB9);                     
    IIC_Wait_Ack( );       
       
    for(i=0;i<4;i++)  
    {  
        dat[i]=IIC_Read_Byte(1);//��ǰ4��������ACK  
    }  
    dat[i]=IIC_Read_Byte(0);//����5��������NACK  
  
    IIC_Stop();//ֹͣ       
}  
  
//��ʪ�ȶ�ȡУ�飬�ɹ�����0xFF,���󷵻�FALSE
void DHT12_ReadData(float *humid, float *temper)  
{   
    DHT12_ReadByte();  

    if(dat[4] == (dat[0]+dat[1]+dat[2]+dat[3]))
	{
		//humid = ((dat[0]*10+dat[1])-200);//ʪ�ȵ�ȡֵ��Χ��20%-95%,ȡ����Ϊ-20,���ֵ��ΧΪ0-750  
                           //����Ľ�������ֵ V_Humid =(����λ+С��λ)/�ֱ���+����; 
		*humid = dat[0] + (float)dat[1]/10;
		
		if(dat[3]&0x80)
		{			
            //temper = (200-(dat[2]*10+(dat[3]&0x7F)));//�¶�Ϊ����,�¶�ȡֵ��Χ��-20-60,����Ϊ200;
			*temper = 0 - (dat[2] + (float)dat[3]/10.0);
		}			
        else
		{			
            //temper = (200+(dat[2]*10+(dat[3]&0x7F)));//�¶�Ϊ����;
			*temper = dat[2] + (float)dat[3]/10.0;
		}
	}		
}  


