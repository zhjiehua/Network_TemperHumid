#include "led.h"
#include "delay.h"
//#include "key.h"
#include "sys.h"
//#include "lcd.h"
#include "usart.h"	 
#include "rtc.h" 
#include "adc.h"
#include "tsensor.h"
#include "dm9000a.h"
#include  "dm9k_uip.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"				   
#include "math.h" 	
#include "string.h"	
#include "dht12.h" 
#include "24cxx.h"
#include "wdg.h"

#include "protocol.h"

#include "uip_demo.h"
#include "httpd.h"
//#include "resolv.h"

//ALIENTEKս��STM32������ʵ��52
//ENC28J60���� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 

/*
*2014.6.13 17:39
*ʵ����TCP_Server��TCP_Client���ܣ�HTTP����ʵ��
*
*TCP_Server ip:		202.202.171.16 
*			port:	1200
*TCP_Client ʱPC��ip:		202.202.171.253
*				  port:		1400		
*/

void uip_polling(void);	 												
#define BUF1 ((struct uip_eth_hdr *)&uip_buf[0])
	
//extern u32 LCD_Pow(u8 m,u8 n);
//����->�ַ���ת������
//��num����(λ��Ϊlen)תΪ�ַ���,�����buf����
//num:����,����
//buf:�ַ�������
//len:����
void num2str(u16 num,u8 *buf,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		//buf[i]=(num/LCD_Pow(10,len-i-1))%10+'0';
	}
}
//��ȡSTM32�ڲ��¶ȴ��������¶�
//temp:����¶��ַ������׵�ַ.��"28.3";
//temp,���ٵ���5���ֽڵĿռ�!
void get_temperature(u8 *temp)
{			  
	u16 t;
	float temperate;		   
	temperate=Get_Adc_Average(ADC_CH_TEMP,10);			 
	temperate=temperate*(3.3/4096);			    											    
	temperate=(1.43-temperate)/0.0043+25;	//�������ǰ�¶�ֵ
	t=temperate*10;//�õ��¶�
	num2str(t/10,temp,2);							   
	temp[2]='.';temp[3]=t%10+'0';temp[4]=0;	//�����ӽ�����
}
//��ȡRTCʱ��
//time:���ʱ���ַ���,����:"2012-09-27 12:33"
//time,���ٵ���17���ֽڵĿռ�!
void get_time(u8 *time)
{	
	RTC_Get();
	time[4]='-';time[7]='-';time[10]=' ';
	time[13]=':';time[16]=0;			//�����ӽ�����
	num2str(calendar.w_year,time,4);	//���->�ַ���
	num2str(calendar.w_month,time+5,2); //�·�->�ַ���	 
	num2str(calendar.w_date,time+8,2); 	//����->�ַ���
	num2str(calendar.hour,time+11,2); 	//Сʱ->�ַ���
	num2str(calendar.min,time+14,2); 	//����->�ַ���								   	  									  
}

//���Ҫ��SWD���ԣ���ע�͵�SWD_DEBUG�꣬��ʱ����ʹ�ûָ�Ĭ�����õĹ���
//����ʹ��ʱ��Ҫ��������
#define SWD_DEBUG

 int main(void)
 {	 
	u16 tcnt=0;
	uint16_t restoreCnt = 0;
	u8 tcp_server_tsta=0XFF;
	u8 tcp_client_tsta=0XFF;
 	uip_ipaddr_t ipaddr;

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

 	LED_Init();			     //LED�˿ڳ�ʼ��	 
	AT24CXX_Init();
	while(AT24CXX_Check())
	{
		LED0 = ~LED0;
		delay_ms(500);
	}
	
	IWDG_Init(4,625);    //Ԥ��Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s
	
	InitUserData(0); //��ʼ���û�����
	
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600

	DHT12_Init();
//	LCD_Init();			 	
//	KEY_Init();				//��ʼ������
//	RTC_Init();				//��ʼ��RTC
//	Adc_Init();				//��ʼ��ADC	  

 	tapdev_init(htSensor.mac);	//��ʼ��dm9000a	
	uip_init();				//uIP��ʼ��	     
 	uip_ipaddr(ipaddr, htSensor.localIP[0], htSensor.localIP[1], htSensor.localIP[2], htSensor.localIP[3]);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, htSensor.gateway[0], htSensor.gateway[1], htSensor.gateway[2], htSensor.gateway[3]); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, htSensor.submask[0], htSensor.submask[1], htSensor.submask[2], htSensor.submask[3]);	//������������
	uip_setnetmask(ipaddr);

//	resolv_init();  
//    uip_ipaddr(ipaddr, 192,168,1,1); //DNS server ,Google DNS Server  
//    resolv_conf(ipaddr);
	
	switch(htSensor.serviceType)
	{
		case SERVICETYPE_TCPSERVER:
			uip_listen(HTONS(htSensor.localPort));			//����1200�˿�,����TCP Server
		break;
		case SERVICETYPE_TCPCLIENT:
			tcp_client_connect();	   		    //�������ӵ�TCP Server��,����TCP Client
		break;
		case SERVICETYPE_UDPSERVER:
			udp_server_connect();				//�������ӵ�UDP Client��,����UDP Server �˿�1600	
		break;
		case SERVICETYPE_UDPCLIENT:
			udp_client_connect();				//�������ӵ�UDP Server��,����UDP Client �˿�1500
			DHT12_ReadData(&htSensor.humid, &htSensor.temper);
			sprintf((char*)udp_client_databuf, "humid = %.1f, temper = %.1f\r\n", htSensor.humid, htSensor.temper);
			udp_client_sta|=1<<5;//�����������Ҫ����
		break;
		default:
		break;
	}

	//uip_listen(HTONS(80));				//����80�˿�,����Web Server

#ifndef SWD_DEBUG	
	{
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��  

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //PA.9
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
		
		GPIO_SetBits(GPIOA, GPIO_Pin_13);
	}
#endif
	
	while(1)
	{
		IWDG_Feed(); //ι��
		
		uip_polling();	//����uip�¼���������뵽�û������ѭ������
		
		tcnt++;
		if(tcnt>1000) 
		{
			tcnt = 0;
			//LED0 = ~LED0;
			LED1 = ~LED1;	

			DHT12_ReadData(&htSensor.humid, &htSensor.temper);

#ifndef SWD_DEBUG			
			if(restorePin == 0)
			{
				restoreCnt++;
				if(restoreCnt >= 50)
				{
					LED0 = 0;
					InitUserData(1);
					SoftReset();
				}
			}
			else
				restoreCnt = 0;
#endif
		}

		switch(htSensor.serviceType)
		{
			case SERVICETYPE_TCPSERVER:
				//====================================TCP Server============================================		
				if(tcp_server_tsta!=tcp_server_sta)//TCP Server״̬�ı�
				{															 
					if(tcp_server_sta&(1<<6))	//�յ�������
					{
						Parse4Pc(tcp_server_databuf, strlen((const char*)tcp_server_databuf)); //��������
						
						tcp_server_sta&=~(1<<6);		//��������Ѿ�������	
					}
					tcp_server_tsta=tcp_server_sta;
				}

				if(htSensor.comType == 1)
				{
					if(htSensor.periodTimer >= htSensor.period)
					{
						htSensor.periodTimer = 0;
						
						sprintf((char*)tcp_server_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
						if(tcp_server_sta&(1<<7))
							tcp_server_sta|=1<<5;//�����������Ҫ����
					}
				}
			break;
			case SERVICETYPE_TCPCLIENT:
				//====================================TCP Client============================================
				if(tcp_client_tsta!=tcp_client_sta)//TCP Client״̬�ı�
				{															 
					if(tcp_client_sta&(1<<6))	//�յ�������
					{
						Parse4Pc(tcp_client_databuf, strlen((const char*)tcp_client_databuf)); //��������

						tcp_client_sta&=~(1<<6);		//��������Ѿ�������	
					}
					tcp_client_tsta=tcp_client_sta;
				}
				
				if(htSensor.comType == 1)
				{
					if(htSensor.periodTimer >= htSensor.period)
					{
						htSensor.periodTimer = 0;
						
						sprintf((char*)tcp_client_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
						if(tcp_client_sta&(1<<7))
							tcp_client_sta|=1<<5;//�����������Ҫ����
					}
				}
			break;
			case SERVICETYPE_UDPSERVER:
				//====================================UDP Server============================================											 
				if(udp_server_sta&(1<<6))	//�յ�������
				{
					Parse4Pc(udp_server_databuf, strlen((const char*)udp_server_databuf)); //��������

					udp_server_sta&=~(1<<6);		//��������Ѿ�������	
				}

				if(htSensor.comType == 1)
				{
					if(htSensor.periodTimer >= htSensor.period)
					{
						htSensor.periodTimer = 0;
						
						sprintf((char*)udp_server_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
						udp_server_sta|=1<<5;//�����������Ҫ����
					}
				}
			break;
			case SERVICETYPE_UDPCLIENT:
				 //====================================UDP Client============================================													 
				if(udp_client_sta&(1<<6))	//�յ�������
				{
					Parse4Pc(udp_client_databuf, strlen((const char*)udp_client_databuf)); //��������

					udp_client_sta&=~(1<<6);		//��������Ѿ�������	
				}
				
				if(htSensor.comType == 1)
				{
					if(htSensor.periodTimer >= htSensor.period)
					{
						htSensor.periodTimer = 0;
						
						sprintf((char*)udp_client_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
						udp_client_sta|=1<<5;//�����������Ҫ����
					}
				}
			break;
			default:
			break;
		}

	} 
} 
 
//uip�¼�������
//���뽫�ú��������û���ѭ��,ѭ������.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ�� 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� 
	}
					 
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���

	if(uip_len>0) 			//������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF1->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
		else if (BUF1->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
					
			uip_arp_arpin();
			
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)
			{
				tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����
			}	 
		}
	}

	else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�  
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len>0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//����UDPͨ���¼�
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}
