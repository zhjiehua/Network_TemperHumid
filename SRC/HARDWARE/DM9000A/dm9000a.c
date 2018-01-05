
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

//uint8_t MAC_Addr[6] = {0x00,0x00,0x00,0x00,0x00,0x00}; //���������ַ
extern uint16_t uip_len;

unsigned char arpsendbuf[42]={   
    
       0xff,0xff,0xff,0xff,0xff,0xff,        //��̫��Ŀ���ַ��ȫ1��ʾΪ�㲥��ַ   
       0x00,0x01,0x02,0x03,0x04,0x05,        //��̫��Դ��ַ   
       0x08,0x06,                            //֡���ͣ�ARP֡   
         
       0x00,0x01,                            //Ӳ�����ͣ���̫��   
       0x08,0x00,                            //Э�����ͣ�IPЭ��   
       0x06,                                 //Ӳ����ַ���ȣ�6�ֽ�   
       0x04,                                 //Э���ַ���ȣ�4�ֽ�   
       0x00,0x01,                            //�����룺ARP����   
         
       0x04,0x02,0x35,0x00,0x00,0x01,        //���Ͷ���̫��Ӳ����ַ   
       192, 168, 1, 16,                      //���Ͷ�IPЭ���ַ   
       0x00,0x30,0x67,0x34,0x00,0x14,        //���ն���̫��Ӳ����ַ 08-10-77-A9-F6-80  
       192, 168, 1, 2                       //���ն�IPЭ���ַ   
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
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
//	/*GPIOG.13 INT�ж����Լ��жϳ�ʼ������   �����ش���*/
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource13);
//  	EXTI_InitStructure.EXTI_Line=EXTI_Line13;	
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
//		
//	/*GPIOG.13 NVIC�ж����ȼ�����*/
//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ��INT���ڵ��ⲿ�ж�ͨ��
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1 
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
		
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

//дDM9000�Ĵ���   
void  dm_reg_write(uint16_t reg, uint16_t data)   
{   
	DM_ADDR_PORT = reg;            //���Ĵ�����ַд����ַ�˿�   
	DM_DATA_PORT = data;            //������д�����ݶ˿�
}   
    
//��DM9000�Ĵ���   
uint8_t  dm_reg_read(uint16_t reg)   
{   
	DM_ADDR_PORT = reg;              
	return (uint8_t) DM_DATA_PORT;             //�����ݴ����ݶ˿ڶ���   
} 

//DM9000�Ĵ�������
void dm_init(void)   
{   
    FSMC_Init(); 
	TIM6_Int_Init(1000,719);//100Khz����Ƶ�ʣ�������1000Ϊ10ms
	 
	dm_reg_write(DM9000_NCR,1);         //�����λDM9000   
    delay_us(300);             				  //��ʱ����20us   
    dm_reg_write(DM9000_NCR,0);         //�����λλ  
    
    dm_reg_write(DM9000_NCR,1);         //Ϊ��ȷ����λ��ȷ���ٴθ�λ    
	delay_us(300);   
    dm_reg_write(DM9000_NCR,0);   
         
    //dm_reg_write(DM9000_GPCR,1);        //����GPIO0Ϊ���     
    dm_reg_write(DM9000_IMR,0x80);
	dm_reg_write(DM9000_GPR,0);         //�����ڲ�PHY

	dm_reg_write(DM9000_NSR,0x2c);      //��TX״̬      
    dm_reg_write(DM9000_ISR,0x3f);      //���ж�״̬
      
    dm_reg_write(DM9000_PAR1,(uint16_t)mymac[0]);     //����MAC��ַ��    
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
    
	//dm_reg_write(DM9000_RCR,0x39);      //����RX����
	dm_reg_write(DM9000_RCR,0x3B);      //����RX����    
    dm_reg_write(DM9000_TCR,0x00);      //����TX����   
	dm_reg_write(DM9000_BPTR,0x3f);     //���ñ�ѹ��ֵ�Ĵ���            
	dm_reg_write(DM9000_FCTR,0x3a);     //������������ֵ�Ĵ���  
	dm_reg_write(DM9000_FCR,0xff);   	  //���ý��շ��������ƼĴ���
	dm_reg_write(DM9000_SMCR,0x00); 	  //��������ģʽ���ƼĴ���

	dm_reg_write(DM9000_NSR,0x2c);      //�ٴ���TX״̬     
	dm_reg_write(DM9000_ISR,0x3f);      //�ٴ����ж�״̬    

    dm_reg_write(DM9000_IMR,0x81);      //�򿪽��������ж�  
} 

//DM9000�������ݰ�
void dm_tran_packet(uint8_t *datas, uint32_t length)   
{   
    uint32_t i;   
    //printf("send start\r\n");     
    dm_reg_write(DM9000_IMR, 0x80);          //�ڷ������ݹ����н�ֹ�����ж�   
    
    dm_reg_write(DM9000_TXPLH, (length>>8) & 0x0ff);           //���÷������ݳ���   
	dm_reg_write(DM9000_TXPLL, length & 0x0ff);   
      
    DM_ADDR_PORT = DM9000_MWCMD;                 //�������ݻ��渳�����ݶ˿�   
         
    //��������   
    for(i=0;i<length;i+=2)   
    {   
       delay_us(50);   
       DM_DATA_PORT = datas[i]|(datas[i+1]<<8);            //8λ����ת��Ϊ16λ�������   
    }       
         
		dm_reg_write(DM9000_TCR, 0x01);          //�����ݷ��͵���̫����
		//printf("before while\r\n");    
		while((dm_reg_read(DM9000_NSR) & 0x0c) == 0);  //�ȴ����ݷ������
		//delay_us(50);   
			
		dm_reg_write(DM9000_NSR, 0x2c);          //���TX״̬ 
		//delay_us(50);
		//printf("complete\r\n");  
		dm_reg_write(DM9000_IMR, 0x81);          //��DM9000���������ж�   
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

       int_status = dm_reg_read(DM9000_ISR);           //��ȡISR   
       if(int_status & 0x1)                     //�ж��Ƿ�������Ҫ����   
       {   
              rx_ready = dm_reg_read(DM9000_MRCMDX);         //�ȶ�ȡһ����Ч������   
              rx_ready = (uint8_t)DM_DATA_PORT;            //������ȡ�������ݰ����ֽ�   
                
              if((rx_ready&0x01) == 1)                 //�ж����ֽ��Ƿ�Ϊ1��0   
              {   
                     DM_ADDR_PORT = DM9000_MRCMD;           //������ȡ���ݰ�����   
    
                     rx_status = DM_DATA_PORT;                           //״̬�ֽ�   
                       
                     rx_length = DM_DATA_PORT;                          //���ݳ���   
                       
                     if(!(rx_status & 0xbf00) && (rx_length < 10000))     //�ж������Ƿ����Ҫ��   
                     {   
                            for(i=0; i<rx_length; i+=2)          //16λ����ת��Ϊ8λ���ݴ洢   
                            {   
                                   delay_us(50);   
                                   temp = DM_DATA_PORT;   
                                   datas[i] = temp & 0x0ff;   
                                   datas[i + 1] = (temp >> 8) & 0x0ff;   
                            }   
                     }   
              }   
              else if((rx_ready&0x02) !=0)      //ֹͣ�豸   
              {   
                     //dm_reg_write(DM9000_IMR,0x80);  //ֹͣ�ж�   
                     //dm_reg_write(DM9000_ISR,0x0F);   //���ж�״̬   
                     //dm_reg_write(DM9000_RCR,0x0);    //ֹͣ����   
                     //����Ҫ��λϵͳ��������ʱû�д���   
              }   
       }   
       dm_reg_write(DM9000_ISR, 0x1);             //���ж�   
       return rx_length;   
}   
//
///*INT�жϷ�����*/
//void EXTI15_10_IRQHandler(void)
//{
//		//uint32_t i;
//		
//		if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
//		{
//			printf("interrupt\r\n");
//			//�жϷ�������
//			uip_len = tapdev_read();
//			//packet_len = dm_rec_packet(buffer);                  //������������   
//    
////            if((buffer[12]==0x08)
////				&&(buffer[12]==0x08)&&(buffer[13]==0x06))          //��ARPЭ��   
////            {              
////                 //ͨ��UART��ʾ����
////			     printf("the packet is :\r\n");   
////                 for(i=0;i<packet_len;i++)   
////                 {   
////                     printf("%d\t",buffer[i]);    
////                 }   
////
////                 flag=0;                  //���־   
////            }   
//            //else if(flag==1)             //����ڷ���ARP������󣬽��յ������ݲ���ARPЭ��   
//            //{   
//            //     comm=3;               //��������ARP�����   
//            //}   
//
//
////LED0 = ~LED0;
//			//LED1 = ~LED1;
//			EXTI_ClearITPendingBit(EXTI_Line13);  //���LINE13�ϵ��жϱ�־λ 
//		}
//     
//}
//
