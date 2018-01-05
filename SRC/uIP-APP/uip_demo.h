#ifndef __UIP_DEMO_H__
#define __UIP_DEMO_H__		 
/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"
#include "psock.h"
#include "sys.h"

//通信程序状态字(用户可以自己定义)  
enum
{
	STATE_CMD		= 0,	//命令接收状态 
	STATE_TX_TEST,	//连续发送数据包状态(速度测试)  
	STATE_RX_TEST,		//连续接收数据包状态(速度测试)  
	CLOSE_CONNECTION,    //关闭连接
	PRODUCT_INFO_SEND,  //发送ALIENTEK消息
	PRODUCT_INFO_OK  //发送ALIENTEK消息成功
};	 
//定义 uip_tcp_appstate_t 数据类型，用户可以添加应用程序需要用到
//成员变量。不要更改结构体类型的名字，因为这个类型名会被uip引用。
//uip.h 中定义的 	struct uip_conn  结构体中引用了 uip_tcp_appstate_t		  
struct uip_demo_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 
typedef struct uip_demo_appstate uip_tcp_appstate_t;
typedef struct uip_demo_appstate uip_udp_appstate_t;



//定义应用程序回调函数 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_demo_appcall //定义回调函数为 tcp_demo_appcall 
#endif

#ifndef UIP_UDP_APPCALL
#define UIP_UDP_APPCALL udp_demo_appcall //定义回调函数为 udp_demo_appcall 
#endif

void tcp_demo_appcall(void);
void tcp_client_demo_appcall(void);
void tcp_server_demo_appcall(void);
/////////////////////////////////////TCP SERVER/////////////////////////////////////
extern u8 tcp_server_databuf[];   		//发送数据缓存	 
extern u8 tcp_server_sta;				//服务端状态   


/////////////////////////////////////TCP CLIENT/////////////////////////////////////
extern u8 tcp_client_databuf[];   		//发送数据缓存	 
extern u8 tcp_client_sta;				//客户端状态   
void tcp_client_connect(void);
////////////////////////////////////////////////////////////////////////////////////


void udp_demo_appcall(void);
void udp_client_demo_appcall(void);
void udp_server_demo_appcall(void);

/////////////////////////////////////UDP SERVER/////////////////////////////////////
extern u8 udp_server_databuf[];   		//发送数据缓存	 
extern u8 udp_server_sta;				//服务端状态   
void udp_server_connect(void);
/////////////////////////////////////UDP CLIENT/////////////////////////////////////
extern u8 udp_client_databuf[];   		//发送数据缓存	 
extern u8 udp_client_sta;				//客户端状态   
void udp_client_connect(void);

#endif



