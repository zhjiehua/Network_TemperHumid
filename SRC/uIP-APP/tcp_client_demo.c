#include "uip_demo.h"
#include "sys.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "dht12.h"
#include "protocol.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK战舰STM32开发板
//uIP TCP Client测试 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/28
//版本：V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////

u8 tcp_client_databuf[200];   	//发送数据缓存	  
u8 tcp_client_sta;				//客户端状态
//[7]:0,无连接;1,已经连接;
//[6]:0,无数据;1,收到客户端数据
//[5]:0,无数据;1,有数据需要发送

//这是一个TCP 客户端应用回调函数。
//该函数通过UIP_APPCALL(tcp_demo_appcall)调用,实现Web Client的功能.
//当uip事件发生时，UIP_APPCALL函数会被调用,根据所属端口(1400),确定是否执行该函数。
//例如 : 当一个TCP连接被创建时、有新的数据到达、数据已经被应答、数据需要重发等事件

#if 0
void tcp_client_demo_appcall(void)
{		  
 	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	if(uip_aborted())tcp_client_aborted();		//连接终止	   
	if(uip_timedout())tcp_client_timedout();	//连接超时   
	if(uip_closed())tcp_client_closed();		//连接关闭	   
 	if(uip_connected())tcp_client_connected();	//连接成功	    
	if(uip_acked())tcp_client_acked();			//发送的数据成功送达 
 	//接收到一个新的TCP数据包 
	if (uip_newdata())
	{
		if((tcp_client_sta&(1<<6))==0)//还未收到数据
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
	    	strcpy((char*)tcp_client_databuf,uip_appdata);				   	  		  
			tcp_client_sta|=1<<6;//表示收到客户端数据
		}				  
	}else if(tcp_client_sta&(1<<5))//有数据需要发送
	{
		s->textptr=tcp_client_databuf;
		s->textlen=strlen((const char*)tcp_client_databuf);

		tcp_client_sta&=~(1<<5);//清除标记
	}  
	//当需要重发、新数据到达、数据包送达、连接建立时，通知uip发送数据 
	if(uip_rexmit()||uip_newdata()||uip_acked()||uip_connected()||uip_poll())
	{
		tcp_client_senddata();
	}											   
}
//这里我们假定Server端的IP地址为:192.168.1.103
//这个IP必须根据Server端的IP修改.
//尝试重新连接
void tcp_client_reconnect()
{
	uip_ipaddr_t ipaddr;
	uip_ipaddr(&ipaddr, htSensor.remoteIP[0], htSensor.remoteIP[1], htSensor.remoteIP[2], htSensor.remoteIP[3]);	//设置IP为192.168.1.103
	uip_connect(&ipaddr,htons(htSensor.port)); 	//端口为1400
}
//终止连接				    
void tcp_client_aborted(void)
{
	tcp_client_sta&=~(1<<7);	//标志没有连接
	tcp_client_reconnect();		//尝试重新连接
	uip_log("tcp_client aborted!\r\n");//打印log
}
//连接超时
void tcp_client_timedout(void)
{
	tcp_client_sta&=~(1<<7);	//标志没有连接	   
	uip_log("tcp_client timeout!\r\n");//打印log
}
//连接关闭
void tcp_client_closed(void)
{
	tcp_client_sta&=~(1<<7);	//标志没有连接
	tcp_client_reconnect();		//尝试重新连接
	uip_log("tcp_client closed!\r\n");//打印log
}	 
//连接建立
void tcp_client_connected(void)
{ 
	char string[70];
	struct tcp_demo_appstate *s=(struct tcp_demo_appstate *)&uip_conn->appstate;
 	tcp_client_sta|=1<<7;		//标志连接成功
  	uip_log("tcp_client connected!\r\n");//打印log
	s->state=STATE_CMD; 		//指令状态
	s->textlen=0;
	sprintf(string, "HTSensor%d(TCP Client) was connected successfully!\r\n", htSensor.productID);//回应消息
	s->textptr=(u8_t *)string;
	s->textlen=strlen((char *)s->textptr);	  
}
//发送的数据成功送达
void tcp_client_acked(void)
{											    
	struct tcp_demo_appstate *s=(struct tcp_demo_appstate *)&uip_conn->appstate;
	s->textlen=0;//发送清零
	uip_log("tcp_client acked!\r\n");//表示成功发送		 
}
//发送数据给服务端
void tcp_client_senddata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//s->textptr:发送的数据包缓冲区指针
	//s->textlen:数据包的大小（单位字节）		   
	if(s->textlen>0)uip_send(s->textptr, s->textlen);//发送TCP数据包	 
}

#else

void tcp_client_demo_appcall(void)
{		 
 	struct uip_demo_appstate *s = (struct uip_demo_appstate *)&uip_conn->appstate;
	char string[70];
	
	if(uip_aborted()){
		uip_log("tcp_client uip_aborted!\r\n");//打印log
		tcp_client_sta&=~(1<<7);	//标志没有连接
		tcp_client_connect();		//重新连接到服务器
	}
 	if(uip_closed()){		//连接关闭了,服务器关闭的时候，会两次进入这个函数
		uip_log("tcp_client uip_closed\r\n");//打印log
		tcp_client_sta&=~(1<<7);	//标志没有连接
		if(s->state!=CLOSE_CONNECTION){
			s->state = CLOSE_CONNECTION;
		}else{
			tcp_client_connect();
		}
	}
	if(uip_timedout()){
		uip_log("tcp_client uip_timedout!\r\n");//打印log
	}	
	
	if(uip_connected()){	//连接成功
		tcp_client_sta|=1<<7;		//标志连接成功
		uip_log("tcp_client connected!\r\n");//打印log	
		s->state = PRODUCT_INFO_SEND;
		sprintf(string, "HTSensor%d(TCP Client) was connected successfully!\r\n", htSensor.productID);//回应消息
		s->textptr=(u8_t *)string;
		s->textlen=strlen((char *)s->textptr);	  
		uip_send(s->textptr, s->textlen);//发送TCP数据包
	}		
	
	if(uip_acked()){	//表示上次发送的数据成功送达 
		uip_log("tcp_client acked!\r\n");//表示成功发送	
		if(s->state == PRODUCT_INFO_SEND){	//收到发送 ALIENTEK 的确认
			s->state = PRODUCT_INFO_OK;
		}			
	}

	if (uip_newdata()&&(s->state == PRODUCT_INFO_OK))					//接收新的TCP数据包 	
	{
		if((tcp_client_sta&(1<<6))==0)//还未收到数据
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
			strcpy((char*)tcp_client_databuf,uip_appdata);				   	  		  
			tcp_client_sta|=1<<6;//表示收到客户端数据
		}				  
	}
		
	if(uip_poll()){	//当前连接空闲轮训
		uip_log("tcp_client uip_poll!\r\n");//打印log
		if(tcp_client_sta&(1<<5)){ //需要发送数据
				s->textptr=tcp_client_databuf;
				s->textlen=strlen((const char*)tcp_client_databuf);
				tcp_client_sta&=~(1<<5);//清除标记
				uip_send(s->textptr, s->textlen);//发送TCP数据包	
		}

	}
	if(uip_rexmit()){ //重发 
		uip_send(s->textptr, s->textlen);//发送TCP数据包		
	}
	
}
//这里我们假定Server端的IP地址为:192.168.1.103
//这个IP必须根据Server端的IP修改.
//尝试重新连接
void tcp_client_connect()
{
	uip_ipaddr_t ipaddr;
	uip_ipaddr(&ipaddr, htSensor.remoteIP[0], htSensor.remoteIP[1], htSensor.remoteIP[2], htSensor.remoteIP[3]);	//设置IP为192.168.1.103
	uip_connect(&ipaddr,HTONS(htSensor.remotePort)); 	//端口为1400
}

#endif

