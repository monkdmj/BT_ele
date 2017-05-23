#include "timer.h"
#include "led.h"
#include "beep.h" 
//stm32f10x_tim
#include "stm32f4xx_tim.h" 
/*********************************************************************************
*************************MCU启明 STM32F407核心开发板******************************
**********************************************************************************
* 文件名称: timer.c                                                              *
* 文件简述：timer定时                                                            *
* 创建日期：2015.03.07                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：定时器2定时到LED1翻转和蜂鸣器响闭                                    * 
**********************************************************************************
*********************************************************************************/ 

/****************************************************************************
* 名    称: TIM2_Init(u16 auto_data,u16 fractional)
* 功    能：定时器2初始化
* 入口参数：auto_data: 自动重装值
*           fractional: 时钟预分频数
* 返回参数：无
* 说    明：定时器溢出时间计算方法:Tout=((auto_data+1)*(fractional+1))/Ft(us)  Ft定时器时钟
****************************************************************************/
float dis[3]={0.0f, 0.0f, 0.0f};

void TIM2_Init(u16 auto_data,u16 fractional)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);      //使能TIM2时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = auto_data; 	     //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=fractional;      //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器2更新中断
	TIM_Cmd(TIM2,DISABLE);                    //使能定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;  //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void HC_SR04_tInit(void)										
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;					
 	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		//PB12--ECHO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		//¸¡¿ÕÊäÈë
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;			//PB13--TRIG	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;					//Ä£ÄâÊä³ö
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOB, GPIO_Pin_12 |GPIO_Pin_13);
	
	TIM2_Init(4999,8399);	//定时器2时钟84M，分频系数8400，84M/8400=10K 所以计数5000次为500ms 
}
 
/*À­¸ßTRIGÒ²ÊÇPB13 15us*/
void SR04_Trg(void)										
{
	Trgger=1;
	delay_us(15);
	Trgger=0;
}

/*»ñÈ¡Ê±¼ä²¢¼ÆËã¾àÀë*/
void SR04_Cnt(void)
{
	u16 tim = 0;
	tim = TIM_GetCounter(TIM2);
	dis[0]=tim*1.7f;  //tim/10000.0f*340/2*100 cm
	TIM2->CNT=0;											
}

void hcrs04_running(void)
{
	SR04_Trg();
	TIM2->CNT=0;
	while(ECHO == 0);
	TIM_Cmd(TIM2, ENABLE);
	while(ECHO == 1);  // while((ReadEcho()==1)&&(TIM2->CNT<TIM2->ARR-10));
	TIM_Cmd(TIM2, DISABLE);
	SR04_Cnt();	
}

//定时器3中断服务函数
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		//LED1=!LED1;//LED1翻转
		//BEEP=!BEEP;
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}
