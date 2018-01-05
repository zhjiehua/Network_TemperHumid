#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__		 

#include "sys.h"

//通信协议
#define STX '!'

#define PFC_READREG		0x00
#define PFC_WRITEREG	0x01
#define PFC_RESET		0x02
#define PFC_RESTORE		0x03
#define PFC_READHT		0x04

#define SERVICETYPE_TCPSERVER	0
#define SERVICETYPE_TCPCLIENT	1
#define SERVICETYPE_UDPSERVER	2
#define SERVICETYPE_UDPCLIENT	3
#define SERVICETYPE_WEBSERVER	4


//内部寄存器地址
#define	PRODUCTID_ADDR		1
#define LOCALIP_ADDR		(PRODUCTID_ADDR+1)
#define REMOTEIP_ADDR		(LOCALIP_ADDR+4)
#define GATEWAY_ADDR		(REMOTEIP_ADDR+4)
#define SUBMASK_ADDR		(GATEWAY_ADDR+4)
#define MAC_ADDR			(SUBMASK_ADDR+4)
#define LOCALPORT_ADDR		(MAC_ADDR+6)
#define REMOTEPORT_ADDR		(LOCALPORT_ADDR+2)
#define SERVICE_ADDR		(REMOTEPORT_ADDR+2)
#define COMTYPE_ADDR		(SERVICE_ADDR+1)
#define PERIOD_ADDR			(COMTYPE_ADDR+1)
#define HUMID_ADDR			(PERIOD_ADDR+2)
#define TEMPER_ADDR			(HUMID_ADDR+4)

#define EEPROMEND_ADDR		(TEMPER_ADDR+4)

typedef struct
{
	uint8_t len;
	uint8_t id;
	uint8_t pfc;
	uint8_t para[20];
}Protocol_TypeDef;

typedef struct
{
	uint8_t productID;
	uint8_t localIP[4];
	uint8_t remoteIP[4];
	uint8_t gateway[4];
	uint8_t submask[4];
	uint8_t mac[6];
	uint16_t localPort;
	uint16_t remotePort;
	
	uint8_t serviceType;
	uint8_t comType;
	uint16_t period;
	float humid;
	float temper;
	
	uint16_t periodTimer;
}HTSensor_TypeDef;

extern HTSensor_TypeDef htSensor;

void SoftReset(void);
void InitUserData(uint8_t restoreFlag);
void Parse4Pc(uint8_t *buffer, uint8_t len);

#endif
























