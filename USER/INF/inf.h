#ifndef __INF_H
#define __INF_H	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 
 							   
void  ADC2_Init(void); 				//ADCͨ����ʼ��
void  ADC3_Init(void); 				//ADCͨ����ʼ��
void  ADC4_Init(void); 				//ADCͨ����ʼ��
void  ADC5_Init(void); 				//ADCͨ����ʼ��

u16  Get_Adc2(u8 ch); 				//���ĳ��ͨ��ֵ 

u16 Get_Adc2_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
#endif 















