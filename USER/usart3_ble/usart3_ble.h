#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 
#define LTE_connect 	PGin(1)     //LTE���ӱ�־  Ϊ1 ���ֻ�APP���������豸���ӳɹ�

//extern u8 APP_mode;

#define UART3_REC_NUM  			300  	//�����������ֽ��� 200
#define UART3_TRS_NUM           300     // TX MAX 1024
extern u8 uart_byte_count;          //uart_byte_countҪС��USART_REC_LEN
extern u8 receive_str[UART3_REC_NUM];
extern u8 send_str[UART3_TRS_NUM];  

extern float voice;
//extern float dis[4];

void LTE_uart3_init(u32 bound);
void uart3SendChars(u8 *str, u16 strlen);

#endif


