#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 
#define LTE_connect 	PGin(1)     //LTE���ӱ�־  Ϊ1 ���ֻ�APP���������豸���ӳɹ�

extern u8 APP_mode;

#define UART6_REC_NUM  			200  	//�����������ֽ��� 200
#define UART6_TRS_NUM           200     // TX MAX 1024
extern u8 uart_byte_count;          //uart_byte_countҪС��USART_REC_LEN
extern u8 receive_str[UART6_REC_NUM];
extern u8 send_str[UART6_TRS_NUM];  

extern float voice;

void uart6_init(u32 bound);
void uart6SendChars(u8 *str, u16 strlen);

#endif


