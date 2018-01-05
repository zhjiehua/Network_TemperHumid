/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: tapdev.c,v 1.8 2006/06/07 08:39:58 adam Exp $
 */	    
#include "tapdev.h"
#include "uip.h"
#include "dm9000a.h"
#include "dm9k_uip.h"
#include "enc28j60.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEKս��STM32������
//uIP��ENC28J60�ĵײ�ӿ� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/28
//�汾��V1.0			   								  
//////////////////////////////////////////////////////////////////////////////////

//���ڹ̶�IP��ַ���ش򿪺��IP���ã�������û�������
#define UIP_DRIPADDR0   192
#define UIP_DRIPADDR1   168
#define UIP_DRIPADDR2   1
#define UIP_DRIPADDR3   15

//MAC��ַ,����Ψһ
//�����������ս��������,������·����,����Ҫ�޸�MAC��ַ��һ��!
const u8 mymac[6]={0x04,0x02,0x35,0x00,0x00,0x01};	//MAC��ַ
																				  
//��������Ӳ����������MAC��ַ 
//����ֵ��0��������1��ʧ�ܣ�
void tapdev_init(uint8_t *myMac)
{   	 
	u8 i;					  				  
	//��IP��ַ��MAC��ַд�뻺����
 	for (i = 0; i < 6; i++)uip_ethaddr.addr[i]=myMac[i];  
	//����uip_setethaddr((struct uip_eth_addr)myMac[0]);
	
	//dm_init();
	
	//etherdev_init();
	
	ENC28J60_Init((u8*)myMac);
}
//��ȡһ������  
uint16_t tapdev_read(void)
{	
	//return dm_rec_packet(uip_buf);	//there is something wrong with the function of dm_rec_packet(),it receives datas very slowly
	
	//return dm9k_receive_packet(uip_buf);
	
	return ENC28J60_Packet_Receive(MAX_FRAMELEN, uip_buf);
}
//����һ������  
void tapdev_send(void)
{
//	dm_tran_packet(uip_buf, uip_len);

//	etherdev_send(uip_buf, uip_len);
	
	ENC28J60_Packet_Send(uip_len, uip_buf);
}












/*---------------------------------------------------------------------------*/
