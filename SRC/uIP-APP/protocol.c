#include "sys.h"
#include "usart.h"	 		   
#include "uip.h"	    

#include "string.h"
#include "24cxx.h" 
#include "protocol.h"
#include "wdg.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK战舰STM32开发板
//uIP TCP测试 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/28
//版本：V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////

#define EEPROM_DEFAULT_DATA (uint8_t)0x99

HTSensor_TypeDef htSensor;

void InitUserData(uint8_t restoreFlag)
{
	uint8_t eepromDefaultData = 0;
	HTSensor_TypeDef htSensorTemp;
	if(!restoreFlag)
	{
		eepromDefaultData= AT24CXX_ReadOneByte(EE_TYPE-1);
		if(eepromDefaultData != EEPROM_DEFAULT_DATA)
			eepromDefaultData = AT24CXX_ReadOneByte(EE_TYPE-1);
		if(eepromDefaultData != EEPROM_DEFAULT_DATA)
			eepromDefaultData = AT24CXX_ReadOneByte(EE_TYPE-1);
	}
	if(eepromDefaultData != EEPROM_DEFAULT_DATA)
	{
		//恢复默认值
		
		htSensorTemp.productID = 0;
		
		htSensorTemp.localIP[0] = 192;
		htSensorTemp.localIP[1] = 168;
		htSensorTemp.localIP[2] = 1;
		htSensorTemp.localIP[3] = 100;
		
//		htSensorTemp.remoteIP[0] = 192;
//		htSensorTemp.remoteIP[1] = 168;
//		htSensorTemp.remoteIP[2] = 1;
//		htSensorTemp.remoteIP[3] = 108;
		htSensorTemp.remoteIP[0] = 120;
		htSensorTemp.remoteIP[1] = 77;
		htSensorTemp.remoteIP[2] = 240;
		htSensorTemp.remoteIP[3] = 133;
		
		htSensorTemp.gateway[0] = 192;
		htSensorTemp.gateway[1] = 168;
		htSensorTemp.gateway[2] = 1;
		htSensorTemp.gateway[3] = 1;
		
		htSensorTemp.submask[0] = 255;
		htSensorTemp.submask[1] = 255;
		htSensorTemp.submask[2] = 255;
		htSensorTemp.submask[3] = 0;
		
		htSensorTemp.mac[0] = 0x04;
		htSensorTemp.mac[1] = 0x02;
		htSensorTemp.mac[2] = 0x35;
		htSensorTemp.mac[3] = 0x00;
		htSensorTemp.mac[4] = 0x00;
		htSensorTemp.mac[5] = 0x01;
		
		htSensorTemp.localPort = 1200;
		htSensorTemp.remotePort = 1400;
//		htSensorTemp.serviceType = 2;
		htSensorTemp.serviceType = 1;
		htSensorTemp.comType = 1;
		htSensorTemp.period = 200;
		
		htSensorTemp.humid = 0;
		htSensorTemp.temper = 0;
		
		IWDG_Feed();
		
		AT24CXX_Write(PRODUCTID_ADDR, (uint8_t*)(&htSensorTemp.productID), 1);
		AT24CXX_Write(LOCALIP_ADDR, (uint8_t*)(htSensorTemp.localIP), 4);
		IWDG_Feed();
		AT24CXX_Write(REMOTEIP_ADDR, (uint8_t*)(htSensorTemp.remoteIP), 4);
		AT24CXX_Write(GATEWAY_ADDR, (uint8_t*)(htSensorTemp.gateway), 4);
		IWDG_Feed();
		AT24CXX_Write(SUBMASK_ADDR, (uint8_t*)(htSensorTemp.submask), 4);
		AT24CXX_Write(MAC_ADDR, (uint8_t*)(htSensorTemp.mac), 6);
		IWDG_Feed();
		AT24CXX_Write(LOCALPORT_ADDR, (uint8_t*)(&htSensorTemp.localPort), 2);
		AT24CXX_Write(REMOTEPORT_ADDR, (uint8_t*)(&htSensorTemp.remotePort), 2);
		IWDG_Feed();
		AT24CXX_Write(SERVICE_ADDR, (uint8_t*)(&htSensorTemp.serviceType), 1);
		AT24CXX_Write(COMTYPE_ADDR, (uint8_t*)(&htSensorTemp.comType), 1);
		IWDG_Feed();
		AT24CXX_Write(PERIOD_ADDR, (uint8_t*)(&htSensorTemp.period), 2);
		//AT24CXX_Write(HUMID_ADDR, (uint8_t*)(&htSensorTemp.humid), 4);
		//AT24CXX_Write(TEMPER_ADDR, (uint8_t*)(&htSensorTemp.temper), 4);
		
		IWDG_Feed();
		
		//记录已经初始化过
		AT24CXX_WriteOneByte(EE_TYPE-1, EEPROM_DEFAULT_DATA);
	}

	//从EEPROM读取数据
	AT24CXX_Read(PRODUCTID_ADDR, (uint8_t*)(&htSensor.productID), 1);
	AT24CXX_Read(LOCALIP_ADDR, (uint8_t*)(htSensor.localIP), 4);
	AT24CXX_Read(REMOTEIP_ADDR, (uint8_t*)(htSensor.remoteIP), 4);
	AT24CXX_Read(GATEWAY_ADDR, (uint8_t*)(htSensor.gateway), 4);
	AT24CXX_Read(SUBMASK_ADDR, (uint8_t*)(htSensor.submask), 4);
	AT24CXX_Read(MAC_ADDR, (uint8_t*)(htSensor.mac), 6);
	AT24CXX_Read(LOCALPORT_ADDR, (uint8_t*)(&htSensor.localPort), 2);
	AT24CXX_Read(REMOTEPORT_ADDR, (uint8_t*)(&htSensor.remotePort), 2);
	if(!restoreFlag) //如果是通过远程恢复出厂设置，则不能修改这2个参数，否则通信不正常
	{
		AT24CXX_Read(SERVICE_ADDR, (uint8_t*)(&htSensor.serviceType), 1);
		AT24CXX_Read(COMTYPE_ADDR, (uint8_t*)(&htSensor.comType), 1);
	}
	AT24CXX_Read(PERIOD_ADDR, (uint8_t*)(&htSensor.period), 2);
	//AT24CXX_Read(HUMID_ADDR, (uint8_t*)(&htSensor.humid), 4);
	//AT24CXX_Read(TEMPER_ADDR, (uint8_t*)(&htSensor.temper), 4);

	IWDG_Feed();
}

void Hex2Ascii(uint8_t *outBuffer, uint8_t *inBuffer, uint8_t len)
{
	uint8_t i, temp;
	for(i=0;i<len;i++)
	{
		temp = (inBuffer[i]&0xF0)>>4;
		if(temp<0x0A)
			outBuffer[2*i] = temp + 0x30;
		else
			outBuffer[2*i] = temp - 0x0A + 0x41;
		
		temp = inBuffer[i]&0x0F;
		if(temp<0x0A)
			outBuffer[2*i+1] = temp + 0x30;
		else
			outBuffer[2*i+1] = temp - 0x0A + 0x41;
	}
	outBuffer[2*i] = '\0';
}

void Ascii2Hex(uint8_t *outBuffer, uint8_t *inBuffer, uint8_t len)
{
	uint8_t i, temp;
	for(i=0;i<len;i++)
	{
		temp = inBuffer[2*i];
		if(temp < 0x40)
			outBuffer[i] = (temp-0x30)<<4;
		else
			outBuffer[i] = (temp-0x41+0x0A)<<4;
		
		temp = inBuffer[2*i+1];
		if(temp < 0x40)
			outBuffer[i] |= temp-0x30;
		else
			outBuffer[i] |= temp-0x41+0x0A;
	}
}

void ReadRegister(uint8_t addr, uint8_t len)
{
	uint8_t hexBuf[100];
	//memcpy((uint8_t*)hexBuf, (uint8_t*)(&htSensor.productID)+addr, len);
	AT24CXX_Read(addr, hexBuf, len);
	
	switch(htSensor.serviceType)
	{
		case SERVICETYPE_TCPSERVER:
			Hex2Ascii(tcp_server_databuf+11, hexBuf, len);
			if(tcp_server_sta&(1<<7))
				tcp_server_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_TCPCLIENT:
			Hex2Ascii(tcp_client_databuf+11, hexBuf, len);
			if(tcp_client_sta&(1<<7))
				tcp_client_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_UDPSERVER:
			Hex2Ascii(udp_server_databuf+11, hexBuf, len);
			udp_server_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_UDPCLIENT:
			Hex2Ascii(udp_client_databuf+11, hexBuf, len);
			udp_client_sta|=1<<5;//标记有数据需要发送
		break;
//		case SERVICETYPE_WEBSERVER:
//			
//		break;
		default:			
		break;
	}
}

void WriteRegister(uint8_t addr, uint8_t len, uint8_t *buffer)
{
	//memcpy((uint8_t*)(&htSensor.productID)+addr, buffer, len);
	
	AT24CXX_Write(addr, buffer, len);
	
	switch(htSensor.serviceType)
	{
		case SERVICETYPE_TCPSERVER:
			sprintf((char*)tcp_server_databuf+2*(len+4)+1, " Write Register Successfully!\r\n");
			if(tcp_server_sta&(1<<7))
				tcp_server_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_TCPCLIENT:
			sprintf((char*)tcp_client_databuf+2*(len+4)+1, " Write Register Successfully!\r\n");
			if(tcp_client_sta&(1<<7))
				tcp_client_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_UDPSERVER:
			sprintf((char*)udp_server_databuf+2*(len+4)+1, " Write Register Successfully!\r\n");
			udp_server_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_UDPCLIENT:
			sprintf((char*)udp_client_databuf+2*(len+4)+1, " Write Register Successfully!\r\n");
			udp_client_sta|=1<<5;//标记有数据需要发送
		break;
//		case SERVICETYPE_WEBSERVER:
//			
//		break;
		default:			
		break;
	}
}

void ReadHumidTemper(void)
{
	switch(htSensor.serviceType)
	{
		case SERVICETYPE_TCPSERVER:
			sprintf((char*)tcp_server_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
			if(tcp_server_sta&(1<<7))
				tcp_server_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_TCPCLIENT:
			sprintf((char*)tcp_client_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
			if(tcp_client_sta&(1<<7))
				tcp_client_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_UDPSERVER:
			sprintf((char*)udp_server_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
			udp_server_sta|=1<<5;//标记有数据需要发送
		break;
		case SERVICETYPE_UDPCLIENT:
			sprintf((char*)udp_client_databuf, "HTSensor%d humid = %.1f, temper = %.1f\r\n", htSensor.productID, htSensor.humid, htSensor.temper);
			udp_client_sta|=1<<5;//标记有数据需要发送
		break;
//		case SERVICETYPE_WEBSERVER:
//			
//		break;
		default:			
		break;
	}
}

void SoftReset(void)
{
	__set_FAULTMASK(1);      //关闭所有中断
	NVIC_SystemReset();		//复位
}
	
//解释PC端命令/数据
void Parse4Pc(uint8_t *buffer, uint8_t len)
{
	uint16_t i;
	uint8_t frameLen;
	uint8_t hexBuf[100];
	Protocol_TypeDef *protocol;
	
	for(i=0;i<len;i++)
	{
		if(buffer[i] == STX) //检验是否是帧头
		{
			buffer++;
			Ascii2Hex(&frameLen, buffer, 2); //得到该帧数据长度
			
			frameLen *= 2;
			Ascii2Hex(hexBuf, buffer, frameLen); //将该帧数据转换成HEX
			protocol = (Protocol_TypeDef*)hexBuf;
			
			if(protocol->id == htSensor.productID)
			{
				switch(protocol->pfc)
				{
					case PFC_READREG:
						ReadRegister(protocol->para[0], protocol->para[1]);
					break;
					case PFC_WRITEREG:
						WriteRegister(protocol->para[0], protocol->len-4, (uint8_t*)(&protocol->para[1]));
					break;
					case PFC_RESET:
						SoftReset();
					break;
					case PFC_RESTORE:
						InitUserData(1);
						SoftReset();
					break;
					case PFC_READHT:
						ReadHumidTemper();
					break;
				}
			}
			
			i += frameLen;
		}
	}
}
