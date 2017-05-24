#ifndef __RS485_H
#define __RS485_H			 
#include "common.h"	 								  

	  	
extern u8 RS485_receive_str[128]; 	//���ջ���,���128���ֽ�
extern u8 RS485_receive_status[128];   //���ջ���,���128���ֽ�.
extern u8 status_len;
extern u8 RS485_receive_alarm[128];   //���ջ���,���128���ֽ�.
extern u8 alarm_len;
extern u8 RS485_byte_count;   			  //���յ������ݳ���
extern u8 RS485_status_success;   			  //�Ƿ���յ�����
extern u8 RS485_alarm_success;   			  //�Ƿ���յ�����

//ģʽ����
#define RS485_TX_EN		PGout(6)	    //485ģʽ����.0,����;1,����.

														 
void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);		 
#endif	   
















