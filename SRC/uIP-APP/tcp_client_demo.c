#include "uip_demo.h"
#include "sys.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "dht12.h"
#include "protocol.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEKս��STM32������
//uIP TCP Client���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/28
//�汾��V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////

u8 tcp_client_databuf[200];   	//�������ݻ���	  
u8 tcp_client_sta;				//�ͻ���״̬
//[7]:0,������;1,�Ѿ�����;
//[6]:0,������;1,�յ��ͻ�������
//[5]:0,������;1,��������Ҫ����

//����һ��TCP �ͻ���Ӧ�ûص�������
//�ú���ͨ��UIP_APPCALL(tcp_demo_appcall)����,ʵ��Web Client�Ĺ���.
//��uip�¼�����ʱ��UIP_APPCALL�����ᱻ����,���������˿�(1400),ȷ���Ƿ�ִ�иú�����
//���� : ��һ��TCP���ӱ�����ʱ�����µ����ݵ�������Ѿ���Ӧ��������Ҫ�ط����¼�

#if 0
void tcp_client_demo_appcall(void)
{		  
 	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	if(uip_aborted())tcp_client_aborted();		//������ֹ	   
	if(uip_timedout())tcp_client_timedout();	//���ӳ�ʱ   
	if(uip_closed())tcp_client_closed();		//���ӹر�	   
 	if(uip_connected())tcp_client_connected();	//���ӳɹ�	    
	if(uip_acked())tcp_client_acked();			//���͵����ݳɹ��ʹ� 
 	//���յ�һ���µ�TCP���ݰ� 
	if (uip_newdata())
	{
		if((tcp_client_sta&(1<<6))==0)//��δ�յ�����
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
	    	strcpy((char*)tcp_client_databuf,uip_appdata);				   	  		  
			tcp_client_sta|=1<<6;//��ʾ�յ��ͻ�������
		}				  
	}else if(tcp_client_sta&(1<<5))//��������Ҫ����
	{
		s->textptr=tcp_client_databuf;
		s->textlen=strlen((const char*)tcp_client_databuf);

		tcp_client_sta&=~(1<<5);//������
	}  
	//����Ҫ�ط��������ݵ�����ݰ��ʹ���ӽ���ʱ��֪ͨuip�������� 
	if(uip_rexmit()||uip_newdata()||uip_acked()||uip_connected()||uip_poll())
	{
		tcp_client_senddata();
	}											   
}
//�������Ǽٶ�Server�˵�IP��ַΪ:192.168.1.103
//���IP�������Server�˵�IP�޸�.
//������������
void tcp_client_reconnect()
{
	uip_ipaddr_t ipaddr;
	uip_ipaddr(&ipaddr, htSensor.remoteIP[0], htSensor.remoteIP[1], htSensor.remoteIP[2], htSensor.remoteIP[3]);	//����IPΪ192.168.1.103
	uip_connect(&ipaddr,htons(htSensor.port)); 	//�˿�Ϊ1400
}
//��ֹ����				    
void tcp_client_aborted(void)
{
	tcp_client_sta&=~(1<<7);	//��־û������
	tcp_client_reconnect();		//������������
	uip_log("tcp_client aborted!\r\n");//��ӡlog
}
//���ӳ�ʱ
void tcp_client_timedout(void)
{
	tcp_client_sta&=~(1<<7);	//��־û������	   
	uip_log("tcp_client timeout!\r\n");//��ӡlog
}
//���ӹر�
void tcp_client_closed(void)
{
	tcp_client_sta&=~(1<<7);	//��־û������
	tcp_client_reconnect();		//������������
	uip_log("tcp_client closed!\r\n");//��ӡlog
}	 
//���ӽ���
void tcp_client_connected(void)
{ 
	char string[70];
	struct tcp_demo_appstate *s=(struct tcp_demo_appstate *)&uip_conn->appstate;
 	tcp_client_sta|=1<<7;		//��־���ӳɹ�
  	uip_log("tcp_client connected!\r\n");//��ӡlog
	s->state=STATE_CMD; 		//ָ��״̬
	s->textlen=0;
	sprintf(string, "HTSensor%d(TCP Client) was connected successfully!\r\n", htSensor.productID);//��Ӧ��Ϣ
	s->textptr=(u8_t *)string;
	s->textlen=strlen((char *)s->textptr);	  
}
//���͵����ݳɹ��ʹ�
void tcp_client_acked(void)
{											    
	struct tcp_demo_appstate *s=(struct tcp_demo_appstate *)&uip_conn->appstate;
	s->textlen=0;//��������
	uip_log("tcp_client acked!\r\n");//��ʾ�ɹ�����		 
}
//�������ݸ������
void tcp_client_senddata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//s->textptr:���͵����ݰ�������ָ��
	//s->textlen:���ݰ��Ĵ�С����λ�ֽڣ�		   
	if(s->textlen>0)uip_send(s->textptr, s->textlen);//����TCP���ݰ�	 
}

#else

void tcp_client_demo_appcall(void)
{		 
 	struct uip_demo_appstate *s = (struct uip_demo_appstate *)&uip_conn->appstate;
	char string[70];
	
	if(uip_aborted()){
		uip_log("tcp_client uip_aborted!\r\n");//��ӡlog
		tcp_client_sta&=~(1<<7);	//��־û������
		tcp_client_connect();		//�������ӵ�������
	}
 	if(uip_closed()){		//���ӹر���,�������رյ�ʱ�򣬻����ν����������
		uip_log("tcp_client uip_closed\r\n");//��ӡlog
		tcp_client_sta&=~(1<<7);	//��־û������
		if(s->state!=CLOSE_CONNECTION){
			s->state = CLOSE_CONNECTION;
		}else{
			tcp_client_connect();
		}
	}
	if(uip_timedout()){
		uip_log("tcp_client uip_timedout!\r\n");//��ӡlog
	}	
	
	if(uip_connected()){	//���ӳɹ�
		tcp_client_sta|=1<<7;		//��־���ӳɹ�
		uip_log("tcp_client connected!\r\n");//��ӡlog	
		s->state = PRODUCT_INFO_SEND;
		sprintf(string, "HTSensor%d(TCP Client) was connected successfully!\r\n", htSensor.productID);//��Ӧ��Ϣ
		s->textptr=(u8_t *)string;
		s->textlen=strlen((char *)s->textptr);	  
		uip_send(s->textptr, s->textlen);//����TCP���ݰ�
	}		
	
	if(uip_acked()){	//��ʾ�ϴη��͵����ݳɹ��ʹ� 
		uip_log("tcp_client acked!\r\n");//��ʾ�ɹ�����	
		if(s->state == PRODUCT_INFO_SEND){	//�յ����� ALIENTEK ��ȷ��
			s->state = PRODUCT_INFO_OK;
		}			
	}

	if (uip_newdata()&&(s->state == PRODUCT_INFO_OK))					//�����µ�TCP���ݰ� 	
	{
		if((tcp_client_sta&(1<<6))==0)//��δ�յ�����
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
			strcpy((char*)tcp_client_databuf,uip_appdata);				   	  		  
			tcp_client_sta|=1<<6;//��ʾ�յ��ͻ�������
		}				  
	}
		
	if(uip_poll()){	//��ǰ���ӿ�����ѵ
		uip_log("tcp_client uip_poll!\r\n");//��ӡlog
		if(tcp_client_sta&(1<<5)){ //��Ҫ��������
				s->textptr=tcp_client_databuf;
				s->textlen=strlen((const char*)tcp_client_databuf);
				tcp_client_sta&=~(1<<5);//������
				uip_send(s->textptr, s->textlen);//����TCP���ݰ�	
		}

	}
	if(uip_rexmit()){ //�ط� 
		uip_send(s->textptr, s->textlen);//����TCP���ݰ�		
	}
	
}
//�������Ǽٶ�Server�˵�IP��ַΪ:192.168.1.103
//���IP�������Server�˵�IP�޸�.
//������������
void tcp_client_connect()
{
	uip_ipaddr_t ipaddr;
	uip_ipaddr(&ipaddr, htSensor.remoteIP[0], htSensor.remoteIP[1], htSensor.remoteIP[2], htSensor.remoteIP[3]);	//����IPΪ192.168.1.103
	uip_connect(&ipaddr,HTONS(htSensor.remotePort)); 	//�˿�Ϊ1400
}

#endif

