
#include "dm9000a.h"
#include "timerx.h"
#include "tapdev.h"

//void delay_ms(uint32_t ms);
//
//enum DM9000_PHY_mode
//{
//    DM9000_10MHD = 0, DM9000_100MHD = 1,
//    DM9000_10MFD = 4, DM9000_100MFD = 5,
//    DM9000_AUTO  = 8, DM9000_1M_HPNA = 0x10
//};

//uint8_t MAC_Addr[6] = {0x00,0x00,0x00,0x00,0x00,0x00}; //本机物理地址
extern uint16_t uip_len;

unsigned char arpsendbuf[42]={   
    
       0xff,0xff,0xff,0xff,0xff,0xff,        //以太网目标地址，全1表示为广播地址   
       0x00,0x01,0x02,0x03,0x04,0x05,        //以太网源地址   
       0x08,0x06,                            //帧类型：ARP帧   
         
       0x00,0x01,                            //硬件类型：以太网   
       0x08,0x00,                            //协议类型：IP协议   
       0x06,                                 //硬件地址长度：6字节   
       0x04,                                 //协议地址长度：4字节   
       0x00,0x01,                            //操作码：ARP请求   
         
       0x04,0x02,0x35,0x00,0x00,0x01,        //发送端以太网硬件地址   
       192, 168, 1, 16,                      //发送端IP协议地址   
       0x00,0x30,0x67,0x34,0x00,0x14,        //接收端以太网硬件地址 08-10-77-A9-F6-80  
       192, 168, 1, 2                       //接收端IP协议地址   
};

uint8_t buffer[42];
uint32_t packet_len = 42;
uint8_t flag = 1;
uint8_t comm = 3;

/*DM9000_FSMC_Init*/
/*SD0 -----FSMC_D0(PD14)
	SD1 -----FSMC_D1(PD15)	
	SD2 -----FSMC_D2(PD0)
	SD3 -----FSMC_D3(PD1)
	SD4 -----FSMC_D4(PE7)
	SD5 -----FSMC_D5(PE8)
	SD6 -----FSMC_D6(PE9)
	SD7 -----FSMC_D7(PE10)
	SD8 -----FSMC_D8(PE11)
	SD9 -----FSMC_D9(PE12)
	SD10-----FSMC_D10(PE13)
	SD11-----FSMC_D11(PE14)
	SD12-----FSMC_D12(PE15)
	SD13-----FSMC_D13(PD8)
	SD14-----FSMC_D14(PD9)
	SD15-----FSMC_15(PD10)

	IOW#-----FSMC_NWE(PD5)
	IOR#-----FSMC_NOE(PD4)
	CMD -----FSMC_A1(PF1)
	CS# -----FSMC_NE4(PG12)
	INT -----(PG13)
	PWRST----(PG11)
*/
void FSMC_Init(void)
{
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef p;
    GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF, ENABLE);

    /*-- GPIO Configuration ------------------------------------------------------*/
    /* SRAM Data lines configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* SRAM Address lines configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                   GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
                                   GPIO_Pin_14 | GPIO_Pin_15;
     GPIO_Init(GPIOF, &GPIO_InitStructure);

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                   GPIO_Pin_4 | GPIO_Pin_5;
     GPIO_Init(GPIOG, &GPIO_InitStructure);

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
     GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* NOE and NWE configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* NE4 configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    /* PWRST configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
	//GPIO_ResetBits(GPIOG, GPIO_Pin_11);
	GPIO_SetBits(GPIOG, GPIO_Pin_11);
		
//	/* INT configuration */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_Init(GPIOG, &GPIO_InitStructure);
//		
//	/*GPIOG.13 INT中断线以及中断初始化配置   上升沿触发*/
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource13);
//  	EXTI_InitStructure.EXTI_Line=EXTI_Line13;	
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//		
//	/*GPIOG.13 NVIC中断优先级设置*/
//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能INT所在的外部中断通道
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1 
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
    /*-- FSMC Configuration ------------------------------------------------------*/
    p.FSMC_AddressSetupTime = 0;
    p.FSMC_AddressHoldTime = 0;
    p.FSMC_DataSetupTime = 4;
    p.FSMC_BusTurnAroundDuration = 0;
    p.FSMC_CLKDivision = 0;
    p.FSMC_DataLatency = 0;
    p.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /* Enable FSMC Bank1_SRAM Bank4 */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

//写DM9000寄存器   
void  dm_reg_write(uint16_t reg, uint16_t data)   
{   
	DM_ADDR_PORT = reg;            //将寄存器地址写到地址端口   
	DM_DATA_PORT = data;            //将数据写到数据端口
}   
    
//读DM9000寄存器   
uint8_t  dm_reg_read(uint16_t reg)   
{   
	DM_ADDR_PORT = reg;              
	return (uint8_t) DM_DATA_PORT;             //将数据从数据端口读出   
} 

//DM9000寄存器配置
void dm_init(void)   
{   
    FSMC_Init(); 
	TIM6_Int_Init(1000,719);//100Khz计数频率，计数到1000为10ms
	 
	dm_reg_write(DM9000_NCR,1);         //软件复位DM9000   
    delay_us(300);             				  //延时至少20us   
    dm_reg_write(DM9000_NCR,0);         //清除复位位  
    
    dm_reg_write(DM9000_NCR,1);         //为了确保复位正确，再次复位    
	delay_us(300);   
    dm_reg_write(DM9000_NCR,0);   
         
    //dm_reg_write(DM9000_GPCR,1);        //设置GPIO0为输出     
    dm_reg_write(DM9000_IMR,0x80);
	dm_reg_write(DM9000_GPR,0);         //激活内部PHY

	dm_reg_write(DM9000_NSR,0x2c);      //清TX状态      
    dm_reg_write(DM9000_ISR,0x3f);      //清中断状态
      
    dm_reg_write(DM9000_PAR1,(uint16_t)mymac[0]);     //设置MAC地址：    
    dm_reg_write(DM9000_PAR2,(uint16_t)mymac[1]);           
    dm_reg_write(DM9000_PAR3,(uint16_t)mymac[2]);   
	dm_reg_write(DM9000_PAR4,(uint16_t)mymac[3]);   
    dm_reg_write(DM9000_PAR5,(uint16_t)mymac[4]);   
	dm_reg_write(DM9000_PAR6,(uint16_t)mymac[5]);
	
//	dm_reg_write(DM9000_MAR,0xFF);
//	dm_reg_write(DM9000_MAR+1,0xFF);
//	dm_reg_write(DM9000_MAR+2,0xFF);
//	dm_reg_write(DM9000_MAR+3,0xFF);
//	dm_reg_write(DM9000_MAR+4,0xFF);
//	dm_reg_write(DM9000_MAR+5,0xFF);
//	dm_reg_write(DM9000_MAR+6,0xFF);
//	dm_reg_write(DM9000_MAR+7,0xFF);
    
	//dm_reg_write(DM9000_RCR,0x39);      //设置RX控制
	dm_reg_write(DM9000_RCR,0x3B);      //设置RX控制    
    dm_reg_write(DM9000_TCR,0x00);      //设置TX控制   
	dm_reg_write(DM9000_BPTR,0x3f);     //设置背压阈值寄存器            
	dm_reg_write(DM9000_FCTR,0x3a);     //设置流控制阈值寄存器  
	dm_reg_write(DM9000_FCR,0xff);   	  //设置接收发送流控制寄存器
	dm_reg_write(DM9000_SMCR,0x00); 	  //设置特殊模式控制寄存器

	dm_reg_write(DM9000_NSR,0x2c);      //再次清TX状态     
	dm_reg_write(DM9000_ISR,0x3f);      //再次清中断状态    

    dm_reg_write(DM9000_IMR,0x81);      //打开接受数据中断  
} 

//DM9000发送数据包
void dm_tran_packet(uint8_t *datas, uint32_t length)   
{   
    uint32_t i;   
    //printf("send start\r\n");     
    dm_reg_write(DM9000_IMR, 0x80);          //在发送数据过程中禁止网卡中断   
    
    dm_reg_write(DM9000_TXPLH, (length>>8) & 0x0ff);           //设置发送数据长度   
	dm_reg_write(DM9000_TXPLL, length & 0x0ff);   
      
    DM_ADDR_PORT = DM9000_MWCMD;                 //发送数据缓存赋予数据端口   
         
    //发送数据   
    for(i=0;i<length;i+=2)   
    {   
       delay_us(50);   
       DM_DATA_PORT = datas[i]|(datas[i+1]<<8);            //8位数据转换为16位数据输出   
    }       
         
		dm_reg_write(DM9000_TCR, 0x01);          //把数据发送到以太网上
		//printf("before while\r\n");    
		while((dm_reg_read(DM9000_NSR) & 0x0c) == 0);  //等待数据发送完成
		//delay_us(50);   
			
		dm_reg_write(DM9000_NSR, 0x2c);          //清除TX状态 
		//delay_us(50);
		//printf("complete\r\n");  
		dm_reg_write(DM9000_IMR, 0x81);          //打开DM9000接收数据中断   
} 

uint32_t dm_rec_packet(uint8_t *datas)   
{   
       uint8_t int_status;   
       uint8_t rx_ready;   
       uint16_t rx_status;   
       uint16_t rx_length = 0;   
       uint16_t temp;   
       uint32_t i;   
		
//		u8 temp1;
//		temp1 = dm_reg_read(DM9000_MRRH);
//		temp1 = dm_reg_read(DM9000_MRRH);    

       int_status = dm_reg_read(DM9000_ISR);           //读取ISR   
       if(int_status & 0x1)                     //判断是否有数据要接受   
       {   
              rx_ready = dm_reg_read(DM9000_MRCMDX);         //先读取一个无效的数据   
              rx_ready = (uint8_t)DM_DATA_PORT;            //真正读取到的数据包首字节   
                
              if((rx_ready&0x01) == 1)                 //判读首字节是否为1或0   
              {   
                     DM_ADDR_PORT = DM9000_MRCMD;           //连续读取数据包内容   
    
                     rx_status = DM_DATA_PORT;                           //状态字节   
                       
                     rx_length = DM_DATA_PORT;                          //数据长度   
                       
                     if(!(rx_status & 0xbf00) && (rx_length < 10000))     //判读数据是否符合要求   
                     {   
                            for(i=0; i<rx_length; i+=2)          //16位数据转换为8位数据存储   
                            {   
                                   delay_us(50);   
                                   temp = DM_DATA_PORT;   
                                   datas[i] = temp & 0x0ff;   
                                   datas[i + 1] = (temp >> 8) & 0x0ff;   
                            }   
                     }   
              }   
              else if((rx_ready&0x02) !=0)      //停止设备   
              {   
                     //dm_reg_write(DM9000_IMR,0x80);  //停止中断   
                     //dm_reg_write(DM9000_ISR,0x0F);   //清中断状态   
                     //dm_reg_write(DM9000_RCR,0x0);    //停止接收   
                     //还需要复位系统，这里暂时没有处理   
              }   
       }   
       dm_reg_write(DM9000_ISR, 0x1);             //清中断   
       return rx_length;   
}   
//
///*INT中断服务函数*/
//void EXTI15_10_IRQHandler(void)
//{
//		//uint32_t i;
//		
//		if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
//		{
//			printf("interrupt\r\n");
//			//中断服务函数体
//			uip_len = tapdev_read();
//			//packet_len = dm_rec_packet(buffer);                  //接收网卡数据   
//    
////            if((buffer[12]==0x08)
////				&&(buffer[12]==0x08)&&(buffer[13]==0x06))          //是ARP协议   
////            {              
////                 //通过UART显示出来
////			     printf("the packet is :\r\n");   
////                 for(i=0;i<packet_len;i++)   
////                 {   
////                     printf("%d\t",buffer[i]);    
////                 }   
////
////                 flag=0;                  //清标志   
////            }   
//            //else if(flag==1)             //如果在发出ARP请求包后，接收到的数据不是ARP协议   
//            //{   
//            //     comm=3;               //继续发送ARP请求包   
//            //}   
//
//
////LED0 = ~LED0;
//			//LED1 = ~LED1;
//			EXTI_ClearITPendingBit(EXTI_Line13);  //清除LINE13上的中断标志位 
//		}
//     
//}
//
