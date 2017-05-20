#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 
#define LTE_connect 	PGin(1)     //LTE连接标志  为1 与手机APP或者其他设备连接成功

extern u8 APP_mode;

#define UART6_REC_NUM  			200  	//定义最大接收字节数 200
#define UART6_TRS_NUM           200     // TX MAX 1024
extern u8 uart_byte_count;          //uart_byte_count要小于USART_REC_LEN
extern u8 receive_str[UART6_REC_NUM];
extern u8 send_str[UART6_TRS_NUM];  

extern float voice;

void uart6_init(u32 bound);
void uart6SendChars(u8 *str, u16 strlen);

#endif


