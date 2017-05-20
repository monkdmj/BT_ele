#ifndef __GSM_H
#define __GSM_H
#include "stdio.h"	
#include "common.h" 

#define UART4_REC_LEN  		1024     //RECEIVE MAX 1024
#define UART4_TRS_LEN          1024     // TX MAX 1024
#define EN_UART4_RX 			1	

extern u8  UART4_RX_BUF[UART4_REC_LEN]; //RECEIVE  
extern u8  UART4_TX_BUF[UART4_TRS_LEN]; //SEND

extern u32 u4_count;

void UART4_Config(void);
//void GPIO_Config(void);
void GSM_START(char* cmd , int timeout);
void u4_printf(char* fmt,...);
void GSM_SendAccData( float data1 , float data2 , float data3);
void GSM_SendPalData( float data1 , float data2 , float data3);
void GSM_SendAngData( float data1 , float data2 , float data3);

void GSM_Init(void);

//void UART4_IRQHandler(void);
#endif
