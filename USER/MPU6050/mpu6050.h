#ifndef __MPU6050_H
#define __MPU6050_H
#include "stdio.h"	
#include "common.h" 
#include "string.h"
#include "stdarg.h"

extern float a[3],w[3],angle[3],T;
extern u8 temp_buf[33],result_buf[33];
extern u32 sign;
extern u8 Temp[33];

extern u32 u6_count;

/*
extern u8 hes;
extern u8 sumone;
extern u8 sumtwo;
extern u8 sumthree;
*/
void USART6_Config(void);
//void USART2_IRQHandler(void);
#endif
