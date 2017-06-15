#ifndef __EIIC_H
#define __EIIC_H
//#include "sys.h" 
#include "common.h"  


//IO��������
#define ESDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	
#define ESDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} 	


//IO��������	 
#define EIIC_SCL    		PBout(6)	 	//SCL
#define EIIC_SDA    		PBout(7) 		//SDA	 
#define EREAD_SDA    	PBin(7)  		//����SDA    

//IIC���в�������
void EIIC_Init(void);                //��ʼ��IIC��IO��				 
void EIIC_Start(void);				//����IIC��ʼ�ź�
void EIIC_Stop(void);	  			//����IICֹͣ�ź�
void EIIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 EIIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 EIIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void EIIC_Ack(void);					//IIC����ACK�ź�
void EIIC_NAck(void);				//IIC������ACK�ź�

void EIIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 EIIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
