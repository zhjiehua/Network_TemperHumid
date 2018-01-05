#ifndef __UIP_DEMO_H__
#define __UIP_DEMO_H__		 
/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"
#include "psock.h"
#include "sys.h"

//ͨ�ų���״̬��(�û������Լ�����)  
enum
{
	STATE_CMD		= 0,	//�������״̬ 
	STATE_TX_TEST,	//�����������ݰ�״̬(�ٶȲ���)  
	STATE_RX_TEST,		//�����������ݰ�״̬(�ٶȲ���)  
	CLOSE_CONNECTION,    //�ر�����
	PRODUCT_INFO_SEND,  //����ALIENTEK��Ϣ
	PRODUCT_INFO_OK  //����ALIENTEK��Ϣ�ɹ�
};	 
//���� uip_tcp_appstate_t �������ͣ��û��������Ӧ�ó�����Ҫ�õ�
//��Ա��������Ҫ���Ľṹ�����͵����֣���Ϊ����������ᱻuip���á�
//uip.h �ж���� 	struct uip_conn  �ṹ���������� uip_tcp_appstate_t		  
struct uip_demo_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 
typedef struct uip_demo_appstate uip_tcp_appstate_t;
typedef struct uip_demo_appstate uip_udp_appstate_t;



//����Ӧ�ó���ص����� 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_demo_appcall //����ص�����Ϊ tcp_demo_appcall 
#endif

#ifndef UIP_UDP_APPCALL
#define UIP_UDP_APPCALL udp_demo_appcall //����ص�����Ϊ udp_demo_appcall 
#endif

void tcp_demo_appcall(void);
void tcp_client_demo_appcall(void);
void tcp_server_demo_appcall(void);
/////////////////////////////////////TCP SERVER/////////////////////////////////////
extern u8 tcp_server_databuf[];   		//�������ݻ���	 
extern u8 tcp_server_sta;				//�����״̬   


/////////////////////////////////////TCP CLIENT/////////////////////////////////////
extern u8 tcp_client_databuf[];   		//�������ݻ���	 
extern u8 tcp_client_sta;				//�ͻ���״̬   
void tcp_client_connect(void);
////////////////////////////////////////////////////////////////////////////////////


void udp_demo_appcall(void);
void udp_client_demo_appcall(void);
void udp_server_demo_appcall(void);

/////////////////////////////////////UDP SERVER/////////////////////////////////////
extern u8 udp_server_databuf[];   		//�������ݻ���	 
extern u8 udp_server_sta;				//�����״̬   
void udp_server_connect(void);
/////////////////////////////////////UDP CLIENT/////////////////////////////////////
extern u8 udp_client_databuf[];   		//�������ݻ���	 
extern u8 udp_client_sta;				//�ͻ���״̬   
void udp_client_connect(void);

#endif



