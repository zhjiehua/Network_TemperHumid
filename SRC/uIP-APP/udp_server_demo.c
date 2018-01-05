#include "uip_demo.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "sys.h"
#include "led.h"
#include "protocol.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK战舰STM32开发板
//uIP UDP Server测试 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2013/10/18
//版本：V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////
			    
u8 udp_server_databuf[200];   	//发送数据缓存	  
u8 udp_server_sta;				//服务端状态
//[7]:0,无连接;1,已经连接;
//[6]:0,无数据;1,收到客户端数据
//[5]:0,无数据;1,有数据需要发送

 	   
//这是一个udp 服务器应用回调函数。
//该函数通过UIP_APPCALL(udp_demo_appcall)调用,实现Web Server的功能.
//当uip事件发生时，UIP_APPCALL函数会被调用,根据所属端口(1200),确定是否执行该函数。
//例如 : 当一个udp连接被创建时、有新的数据到达、数据已经被应答、数据需要重发等事件
void udp_server_demo_appcall(void)
{
 	struct uip_demo_appstate *s = (struct uip_demo_appstate *)&uip_udp_conn->appstate;
	
	//接收到一个新的udp数据包 
	if (uip_newdata())//收到客户端发过来的数据
	{
		if((udp_server_sta&(1<<6))==0)//还未收到数据
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
	    	strcpy((char*)udp_server_databuf,uip_appdata);				   	  		  
			udp_server_sta|=1<<6;//表示收到客户端数据
		}
	}
	if(uip_poll())
	{
		uip_log("udp_server uip_poll!\r\n");//打印log
		if(udp_server_sta&(1<<5))//有数据需要发送
		{
			s->textptr=udp_server_databuf;
			s->textlen=strlen((const char*)udp_server_databuf);
			udp_server_sta&=~(1<<5);//清除标记
			uip_send(s->textptr, s->textlen);//发送udp数据包	
			uip_udp_send(s->textlen);
		}   
	}
}	  

//建立一个udp_server连接
void udp_server_connect()
{
	uip_ipaddr_t ipaddr;
	static struct uip_udp_conn *c=0;	
	//uip_ipaddr(&ipaddr,0xff,0xff,0xff,0xff);	//将远程IP设置为 255.255.255.255 具体原理见uip.c的源码
	uip_ipaddr(&ipaddr, htSensor.remoteIP[0], htSensor.remoteIP[1], htSensor.remoteIP[2], htSensor.remoteIP[3]);
	if(c!=0){							//已经建立连接则删除连接
		uip_udp_remove(c);
	}
	//c = uip_udp_new(&ipaddr,0); 	//远程端口为0
	c = uip_udp_new(&ipaddr, HTONS(htSensor.remotePort));
	if(c){
		uip_udp_bind(c, HTONS(htSensor.localPort));
	}
	
}















