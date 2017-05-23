#ifndef _TIMER_H
#define _TIMER_H
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void TIM2_Init(u16 auto_data,u16 fractional);

#define Trgger PBout(7)
#define ECHO PBin(6)

#define Trgger1 PCout(3)
#define ECHO1 PCin(2)

#define Trgger2 PCout(1)
#define ECHO2 PCin(0)

void HC_SR04_tInit(void);

void SR04_Trg(void);
void SR04_Trg1(void);
void SR04_Trg2(void);
void SR04_Cnt(void);
void SR04_Cnt1(void);
void SR04_Cnt2(void);

void hcrs04_running(void);
void hcrs04_running1(void);
void hcrs04_running2(void);

extern float dis[3];				        //¾àÀë
	
#endif
