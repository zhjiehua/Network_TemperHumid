#include "sys.h"
#include "usart.h"	 		   
#include "uip.h"	    
//#include "dm9000a.h"
#include  "dm9k_uip.h"
#include "httpd.h"
#include "uip_demo.h"
#include "protocol.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK战舰STM32开发板
//uIP TCP测试 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/28
//版本：V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////

//TCP应用接口函数(UIP_APPCALL)
//完成TCP服务(包括server和client)和HTTP服务
#if 0
void tcp_demo_appcall(void)
{		
	switch(uip_conn->lport)//本地监听端口80和1200 
	{
		case HTONS(80):
			httpd_appcall(); 
			break;
		case HTONS(1200):
		    tcp_server_demo_appcall(); 
			break;
		default:						  
		    break;
	}		    
	switch(uip_conn->rport)	//远程连接1400端口
	{
	    case HTONS(1400):
			tcp_client_demo_appcall();
	       break;
	    default: 
	       break;
	}   
}

void udp_demo_appcall(void){

	switch(uip_udp_conn->lport)//本地监听端口1600 
	{
		case HTONS(1600):
		    udp_server_demo_appcall(); 
			break;
		default:						  
		    break;
	}		    
	switch(uip_udp_conn->rport)	//远程连接1500端口
	{
	    case HTONS(1500):
			udp_client_demo_appcall();
	       break;
	    default: 
	       break;
	}   
}
#else
void tcp_demo_appcall(void)
{		
	if(uip_conn->lport == HTONS(htSensor.localPort))//本地监听端口80和1200 
		tcp_server_demo_appcall(); 
	if(uip_conn->lport == HTONS(80))
		httpd_appcall();
	    
	if(uip_conn->rport == HTONS(htSensor.remotePort))	//远程连接1400端口
		tcp_client_demo_appcall();
}

void udp_demo_appcall(void){

	if(uip_udp_conn->lport == HTONS(htSensor.localPort))//本地监听端口1600 
		udp_server_demo_appcall(); 
	    
	if(uip_udp_conn->rport == HTONS(htSensor.remotePort))	//远程连接1500端口
		udp_client_demo_appcall();
}
#endif

//打印日志用
void uip_log(char *m)
{			    
	printf("uIP log:%s\r\n",m);
}

























