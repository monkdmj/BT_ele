#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 
#define LTE_connect 	PGin(1)     //LTE连接标志  为1 与手机APP或者其他设备连接成功

//extern u8 APP_mode;

#define UART3_REC_NUM  			300  	//定义最大接收字节数 200
#define UART3_TRS_NUM           300     // TX MAX 1024
extern u8 uart_byte_count;          //uart_byte_count要小于USART_REC_LEN
extern u8 receive_str[UART3_REC_NUM];
extern u8 send_str[UART3_TRS_NUM];  

extern float voice;
//extern float dis[4];

void LTE_uart3_init(u32 bound);
void uart3SendChars(u8 *str, u16 strlen);

#endif


