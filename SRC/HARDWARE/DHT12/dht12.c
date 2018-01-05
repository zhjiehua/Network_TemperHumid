#include "dht12.h" 
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//24CXX驱动 代码(适合24C01~24C16,24C32~256未经过测试!有待验证!)		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

static uint8_t dat[5];//接受数据

//uint16_t humid, temper;
float humid, temper;
  
//初始化IIC接口
void DHT12_Init(void)  
{  
    IIC_Init();  
}  
  
static void DHT12_ReadByte(void)  
{                   
    uint8_t i;                                                                                 
	IIC_Start( ); //主机发送总线开始信号 
  
    IIC_Send_Byte(0xB8);          
    IIC_Wait_Ack( );  
  
    IIC_Send_Byte(0x00);
    IIC_Wait_Ack( );              
  
    IIC_Start( );//进入接收模式，接收0xB8的数据  
    IIC_Send_Byte(0xB9);                     
    IIC_Wait_Ack( );       
       
    for(i=0;i<4;i++)  
    {  
        dat[i]=IIC_Read_Byte(1);//读前4个，发送ACK  
    }  
    dat[i]=IIC_Read_Byte(0);//读第5个，发送NACK  
  
    IIC_Stop();//停止       
}  
  
//温湿度读取校验，成功返回0xFF,错误返回FALSE
void DHT12_ReadData(float *humid, float *temper)  
{   
    DHT12_ReadByte();  

    if(dat[4] == (dat[0]+dat[1]+dat[2]+dat[3]))
	{
		//humid = ((dat[0]*10+dat[1])-200);//湿度的取值范围是20%-95%,取增量为-20,相对值范围为0-750  
                           //计算的结果是相对值 V_Humid =(整数位+小数位)/分辨率+增量; 
		*humid = dat[0] + (float)dat[1]/10;
		
		if(dat[3]&0x80)
		{			
            //temper = (200-(dat[2]*10+(dat[3]&0x7F)));//温度为负数,温度取值范围是-20-60,增量为200;
			*temper = 0 - (dat[2] + (float)dat[3]/10.0);
		}			
        else
		{			
            //temper = (200+(dat[2]*10+(dat[3]&0x7F)));//温度为正数;
			*temper = dat[2] + (float)dat[3]/10.0;
		}
	}		
}  


