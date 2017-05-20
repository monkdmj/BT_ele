#ifndef __INF_H
#define __INF_H	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 
 							   
void  ADC2_Init(void); 				//ADC通道初始化
void  ADC3_Init(void); 				//ADC通道初始化
void  ADC4_Init(void); 				//ADC通道初始化
void  ADC5_Init(void); 				//ADC通道初始化

u16  Get_Adc2(u8 ch); 				//获得某个通道值 

u16 Get_Adc2_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  
#endif 















