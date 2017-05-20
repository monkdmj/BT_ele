#ifndef __USART1_H
#define __USART1_H
#include "stdio.h"	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 

#define USART1_REC_NUM  			200  	//定义最大接收字节数 200
#define USART1_REC_MASTER_NUM  			300  	//定义最大接收字节数 200
#define UART1_TRS_NUM         200     // TX MAX 1024
extern u8 uart1_byte_count;          //uart_byte_count要小于USART_REC_LEN
extern u8 receive_str1[USART1_REC_NUM];  
extern u8 package_data[USART1_REC_MASTER_NUM];
extern u8 send_str1[UART1_TRS_NUM];  

void uart1_init(u32 bound);
void uart1SendChars(u8 *str, u16 strlen);

#endif


