#include "sys.h"
#include "usart.h"	 		   
#include "uip.h"	    
//#include "dm9000a.h"
#include  "dm9k_uip.h"
#include "httpd.h"
#include "uip_demo.h"
#include "protocol.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEKս��STM32������
//uIP TCP���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/28
//�汾��V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////

//TCPӦ�ýӿں���(UIP_APPCALL)
//���TCP����(����server��client)��HTTP����
#if 0
void tcp_demo_appcall(void)
{		
	switch(uip_conn->lport)//���ؼ����˿�80��1200 
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
	switch(uip_conn->rport)	//Զ������1400�˿�
	{
	    case HTONS(1400):
			tcp_client_demo_appcall();
	       break;
	    default: 
	       break;
	}   
}

void udp_demo_appcall(void){

	switch(uip_udp_conn->lport)//���ؼ����˿�1600 
	{
		case HTONS(1600):
		    udp_server_demo_appcall(); 
			break;
		default:						  
		    break;
	}		    
	switch(uip_udp_conn->rport)	//Զ������1500�˿�
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
	if(uip_conn->lport == HTONS(htSensor.localPort))//���ؼ����˿�80��1200 
		tcp_server_demo_appcall(); 
	if(uip_conn->lport == HTONS(80))
		httpd_appcall();
	    
	if(uip_conn->rport == HTONS(htSensor.remotePort))	//Զ������1400�˿�
		tcp_client_demo_appcall();
}

void udp_demo_appcall(void){

	if(uip_udp_conn->lport == HTONS(htSensor.localPort))//���ؼ����˿�1600 
		udp_server_demo_appcall(); 
	    
	if(uip_udp_conn->rport == HTONS(htSensor.remotePort))	//Զ������1500�˿�
		udp_client_demo_appcall();
}
#endif

//��ӡ��־��
void uip_log(char *m)
{			    
	printf("uIP log:%s\r\n",m);
}

























