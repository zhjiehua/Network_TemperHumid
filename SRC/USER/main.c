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

//ALIENTEK战舰STM32开发板实验52
//ENC28J60网络 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 

/*
*2014.6.13 17:39
*实现了TCP_Server、TCP_Client功能，HTTP还待实现
*
*TCP_Server ip:		202.202.171.16 
*			port:	1200
*TCP_Client 时PC的ip:		202.202.171.253
*				  port:		1400		
*/

void uip_polling(void);	 												
#define BUF1 ((struct uip_eth_hdr *)&uip_buf[0])
	
//extern u32 LCD_Pow(u8 m,u8 n);
//数字->字符串转换函数
//将num数字(位数为len)转为字符串,存放在buf里面
//num:数字,整形
//buf:字符串缓存
//len:长度
void num2str(u16 num,u8 *buf,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		//buf[i]=(num/LCD_Pow(10,len-i-1))%10+'0';
	}
}
//获取STM32内部温度传感器的温度
//temp:存放温度字符串的首地址.如"28.3";
//temp,最少得有5个字节的空间!
void get_temperature(u8 *temp)
{			  
	u16 t;
	float temperate;		   
	temperate=Get_Adc_Average(ADC_CH_TEMP,10);			 
	temperate=temperate*(3.3/4096);			    											    
	temperate=(1.43-temperate)/0.0043+25;	//计算出当前温度值
	t=temperate*10;//得到温度
	num2str(t/10,temp,2);							   
	temp[2]='.';temp[3]=t%10+'0';temp[4]=0;	//最后添加结束符
}
//获取RTC时间
//time:存放时间字符串,形如:"2012-09-27 12:33"
//time,最少得有17个字节的空间!
void get_time(u8 *time)
{	
	RTC_Get();
	time[4]='-';time[7]='-';time[10]=' ';
	time[13]=':';time[16]=0;			//最后添加结束符
	num2str(calendar.w_year,time,4);	//年份->字符串
	num2str(calendar.w_month,time+5,2); //月份->字符串	 
	num2str(calendar.w_date,time+8,2); 	//日期->字符串
	num2str(calendar.hour,time+11,2); 	//小时->字符串
	num2str(calendar.min,time+14,2); 	//分钟->字符串								   	  									  
}

//如果要用SWD调试，则注释掉SWD_DEBUG宏，这时不能使用恢复默认设置的功能
//正常使用时需要添加这个宏
#define SWD_DEBUG

 int main(void)
 {	 
	u16 tcnt=0;
	uint16_t restoreCnt = 0;
	u8 tcp_server_tsta=0XFF;
	u8 tcp_client_tsta=0XFF;
 	uip_ipaddr_t ipaddr;

	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级

 	LED_Init();			     //LED端口初始化	 
	AT24CXX_Init();
	while(AT24CXX_Check())
	{
		LED0 = ~LED0;
		delay_ms(500);
	}
	
	IWDG_Init(4,625);    //预分频数为64,重载值为625,溢出时间为1s
	
	InitUserData(0); //初始化用户数据
	
	uart_init(115200);	 	//串口初始化为9600

	DHT12_Init();
//	LCD_Init();			 	
//	KEY_Init();				//初始化按键
//	RTC_Init();				//初始化RTC
//	Adc_Init();				//初始化ADC	  

 	tapdev_init(htSensor.mac);	//初始化dm9000a	
	uip_init();				//uIP初始化	     
 	uip_ipaddr(ipaddr, htSensor.localIP[0], htSensor.localIP[1], htSensor.localIP[2], htSensor.localIP[3]);	//设置本地设置IP地址
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, htSensor.gateway[0], htSensor.gateway[1], htSensor.gateway[2], htSensor.gateway[3]); 	//设置网关IP地址(其实就是你路由器的IP地址)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, htSensor.submask[0], htSensor.submask[1], htSensor.submask[2], htSensor.submask[3]);	//设置网络掩码
	uip_setnetmask(ipaddr);

//	resolv_init();  
//    uip_ipaddr(ipaddr, 192,168,1,1); //DNS server ,Google DNS Server  
//    resolv_conf(ipaddr);
	
	switch(htSensor.serviceType)
	{
		case SERVICETYPE_TCPSERVER:
			uip_listen(HTONS(htSensor.localPort));			//监听1200端口,用于TCP Server
		break;
		case SERVICETYPE_TCPCLIENT:
			tcp_client_connect();	   		    //尝试连接到TCP Server端,用于TCP Client
		break;
		case SERVICETYPE_UDPSERVER:
			udp_server_connect();				//尝试连接到UDP Client端,用于UDP Server 端口1600	
		break;
		case SERVICETYPE_UDPCLIENT:
			udp_client_connect();				//尝试连接到UDP Server端,用于UDP Client 端口1500
			DHT12_ReadData(&htSensor.humid, &htSensor.temper);
			sprintf((char*)udp_client_databuf, "humid = %.1f, temper = %.1f\r\n", htSensor.humid, htSensor.temper);
			udp_client_sta|=1<<5;//标记有数据需要发送
		break;
		default:
		break;
	}

	//uip_listen(HTONS(80));				//监听80端口,用于Web Server

#ifndef SWD_DEBUG	
	{
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟  

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //PA.9
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//下拉输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
		
		GPIO_SetBits(GPIOA, GPIO_Pin_13);
	}
#endif
	
	while(1)
	{
		IWDG_Feed(); //喂狗
		
		uip_polling();	//处理uip事件，必须插入到用户程序的循环体中
		
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
				if(tcp_server_tsta!=tcp_server_sta)//TCP Server状态改变
				{															 
					if(tcp_server_sta&(1<<6))	//收到新数据
					{
						Parse4Pc(tcp_server_databuf, strlen((const char*)tcp_server_databuf)); //解释数据
						
						tcp_server_sta&=~(1<<6);		//标记数据已经被处理	
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
							tcp_server_sta|=1<<5;//标记有数据需要发送
					}
				}
			break;
			case SERVICETYPE_TCPCLIENT:
				//====================================TCP Client============================================
				if(tcp_client_tsta!=tcp_client_sta)//TCP Client状态改变
				{															 
					if(tcp_client_sta&(1<<6))	//收到新数据
					{
						Parse4Pc(tcp_client_databuf, strlen((const char*)tcp_client_databuf)); //解释数据

						tcp_client_sta&=~(1<<6);		//标记数据已经被处理	
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
							tcp_client_sta|=1<<5;//标记有数据需要发送
					}
				}
			break;
			case SERVICETYPE_UDPSERVER:
				//====================================UDP Server============================================											 
				if(udp_server_sta&(1<<6))	//收到新数据
				{
					Parse4Pc(udp_server_databuf, strlen((const char*)udp_server_databuf)); //解释数据

					udp_server_sta&=~(1<<6);		//标记数据已经被处理	
				}

				if(htSensor.comType == 1)
				{
					if(htSensor.periodTimer >= htSensor.period)
					{
						htSensor.periodTimer = 0;
						
						sprintf((char*)udp_server_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
						udp_server_sta|=1<<5;//标记有数据需要发送
					}
				}
			break;
			case SERVICETYPE_UDPCLIENT:
				 //====================================UDP Client============================================													 
				if(udp_client_sta&(1<<6))	//收到新数据
				{
					Parse4Pc(udp_client_databuf, strlen((const char*)udp_client_databuf)); //解释数据

					udp_client_sta&=~(1<<6);		//标记数据已经被处理	
				}
				
				if(htSensor.comType == 1)
				{
					if(htSensor.periodTimer >= htSensor.period)
					{
						htSensor.periodTimer = 0;
						
						sprintf((char*)udp_client_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
						udp_client_sta|=1<<5;//标记有数据需要发送
					}
				}
			break;
			default:
			break;
		}

	} 
} 
 
//uip事件处理函数
//必须将该函数插入用户主循环,循环调用.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 
	}
					 
	uip_len=tapdev_read();	//从网络设备读取一个IP包,得到数据长度.uip_len在uip.c中定义

	if(uip_len>0) 			//有数据
	{   
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(BUF1->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		    
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
		else if (BUF1->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
					
			uip_arp_arpin();
			
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)
			{
				tapdev_send();//需要发送数据,则通过tapdev_send发送
			}	 
		}
	}

	else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
	{
		timer_reset(&periodic_timer);		//复位0.5秒定时器 
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//处理TCP通信事件  
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
	 		if(uip_len>0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#if UIP_UDP	//UIP_UDP 
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//处理UDP通信事件
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#endif 
		//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}
