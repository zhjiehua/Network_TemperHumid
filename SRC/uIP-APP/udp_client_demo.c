#include "uip_demo.h"
#include "sys.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>	
#include "protocol.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEKս��STM32������
//uIP UDP Client���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2013/11/18
//�汾��V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////

u8 udp_client_databuf[200];   	//�������ݻ���	  
u8 udp_client_sta;				//�ͻ���״̬
//[7]:0,������;1,�Ѿ�����;
//[6]:0,������;1,�յ��ͻ�������
//[5]:0,������;1,��������Ҫ����

//����һ��udp �ͻ���Ӧ�ûص�������
//�ú���ͨ��UIP_APPCALL(udp_demo_appcall)����,ʵ��Web Client�Ĺ���.
//��uip�¼�����ʱ��UIP_APPCALL�����ᱻ����,���������˿�(1400),ȷ���Ƿ�ִ�иú�����
//���� : ��һ��udp���ӱ�����ʱ�����µ����ݵ�������Ѿ���Ӧ��������Ҫ�ط����¼�
void udp_client_demo_appcall(void)
{		 
 	struct uip_demo_appstate *s = (struct uip_demo_appstate *)&uip_udp_conn->appstate;

	if (uip_newdata())					//�����µ�udp���ݰ� 	
	{
		if((udp_client_sta&(1<<6))==0)//��δ�յ�����
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
			strcpy((char*)udp_client_databuf,uip_appdata);				   	  		  
			udp_client_sta|=1<<6;//��ʾ�յ��ͻ�������
		}				  
	} 
	
	if(uip_poll()){	//��ǰ���ӿ�����ѵ
		uip_log("udp_client uip_poll!\r\n");//��ӡlog
		if(udp_client_sta&(1<<5)){ //��Ҫ��������
				s->textptr=udp_client_databuf;
				s->textlen=strlen((const char*)udp_client_databuf);
				udp_client_sta&=~(1<<5);//������
				uip_send(s->textptr, s->textlen);//����udp���ݰ�	
				uip_udp_send(s->textlen);
		}

	}
	
}

//����һ��udp_client������
void udp_client_connect()
{
	uip_ipaddr_t ipaddr;
	static struct uip_udp_conn *c=0;	
	uip_ipaddr(&ipaddr, htSensor.remoteIP[0], htSensor.remoteIP[1] , htSensor.remoteIP[2], htSensor.remoteIP[3]);	//����IPΪ192.168.1.103
	if(c!=0){							//�Ѿ�����������ɾ������
		uip_udp_remove(c);
	}
	c = uip_udp_new(&ipaddr, HTONS(htSensor.remotePort)); 	//�˿�Ϊ1500
}



















