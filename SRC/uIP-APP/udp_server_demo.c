#include "uip_demo.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "sys.h"
#include "led.h"
#include "protocol.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEKս��STM32������
//uIP UDP Server���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2013/10/18
//�汾��V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////
			    
u8 udp_server_databuf[200];   	//�������ݻ���	  
u8 udp_server_sta;				//�����״̬
//[7]:0,������;1,�Ѿ�����;
//[6]:0,������;1,�յ��ͻ�������
//[5]:0,������;1,��������Ҫ����

 	   
//����һ��udp ������Ӧ�ûص�������
//�ú���ͨ��UIP_APPCALL(udp_demo_appcall)����,ʵ��Web Server�Ĺ���.
//��uip�¼�����ʱ��UIP_APPCALL�����ᱻ����,���������˿�(1200),ȷ���Ƿ�ִ�иú�����
//���� : ��һ��udp���ӱ�����ʱ�����µ����ݵ�������Ѿ���Ӧ��������Ҫ�ط����¼�
void udp_server_demo_appcall(void)
{
 	struct uip_demo_appstate *s = (struct uip_demo_appstate *)&uip_udp_conn->appstate;
	
	//���յ�һ���µ�udp���ݰ� 
	if (uip_newdata())//�յ��ͻ��˷�����������
	{
		if((udp_server_sta&(1<<6))==0)//��δ�յ�����
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
	    	strcpy((char*)udp_server_databuf,uip_appdata);				   	  		  
			udp_server_sta|=1<<6;//��ʾ�յ��ͻ�������
		}
	}
	if(uip_poll())
	{
		uip_log("udp_server uip_poll!\r\n");//��ӡlog
		if(udp_server_sta&(1<<5))//��������Ҫ����
		{
			s->textptr=udp_server_databuf;
			s->textlen=strlen((const char*)udp_server_databuf);
			udp_server_sta&=~(1<<5);//������
			uip_send(s->textptr, s->textlen);//����udp���ݰ�	
			uip_udp_send(s->textlen);
		}   
	}
}	  

//����һ��udp_server����
void udp_server_connect()
{
	uip_ipaddr_t ipaddr;
	static struct uip_udp_conn *c=0;	
	//uip_ipaddr(&ipaddr,0xff,0xff,0xff,0xff);	//��Զ��IP����Ϊ 255.255.255.255 ����ԭ���uip.c��Դ��
	uip_ipaddr(&ipaddr, htSensor.remoteIP[0], htSensor.remoteIP[1], htSensor.remoteIP[2], htSensor.remoteIP[3]);
	if(c!=0){							//�Ѿ�����������ɾ������
		uip_udp_remove(c);
	}
	//c = uip_udp_new(&ipaddr,0); 	//Զ�̶˿�Ϊ0
	c = uip_udp_new(&ipaddr, HTONS(htSensor.remotePort));
	if(c){
		uip_udp_bind(c, HTONS(htSensor.localPort));
	}
	
}















