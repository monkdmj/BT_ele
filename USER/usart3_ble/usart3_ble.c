#include "usart3_ble.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"  
#include "led.h" 
#include "lcd.h"
#include "beep.h" 
#include "rtc.h"
#include "mpu6050.h"
#include "adc.h"
#include "usart1.h"

/*********************************************************************************
*********************MCU启明 STM32F407应用开发板(增强版)**************************
**********************************************************************************
* 文件名称: lte.c                                                                *
* 文件简述：蓝牙使用                                                             *
* 创建日期：2015.10.06                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：利用手机APP控制开发板                                                * 
**********************************************************************************
*********************************************************************************/	

u8 receive_str[UART3_REC_NUM];     //接收缓存数组,最大USART_REC_LEN个字节 
u8 send_str[UART3_TRS_NUM]; 
u8 uart_byte_count=0;
u8 APP_mode=0;          //APP控制模式  0：命令控制区  1：接收发送区

//float voice = 0.0f;
//float dis1 = 0.0f;
/****************************************************************************
* 名    称: void LTE_uart4_init(u32 bound)
* 功    能：LTE_uart4初始化
* 入口参数：bound：波特率   
* 返回参数：无
* 说    明： 
****************************************************************************/
void LTE_uart3_init(u32 bound)
{   
/****************************** 串口3初始化*********************************/
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //使能USART3时钟 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);  //GPIOB10复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	//USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10与GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);          //初始化PB10，PB11
   //USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口1	
    USART_Cmd(USART3, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);         //开启相关中断
	//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;      //串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		   //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	  //根据指定的参数初始化VIC寄存器、
/****************************** 串口3初始化**********************************/

/******************************* 蓝牙IO口初始化******************************/
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    //GPIOA7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;             //普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;       //100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;          
	GPIO_Init(GPIOG,&GPIO_InitStructure);          //初始化PA9，PA10
}

//串口1发送一个字符
void uart3SendChar(u8 ch)
{      
	while((USART3->SR&0x40)==0);  
    USART3->DR = (u8) ch;      
}
/****************************************************************************
* 名    称: void uart1SendChars(u8 *str, u16 strlen)
* 功    能：串口1发送一字符串
* 入口参数：*str：发送的字符串
            strlen：字符串长度
* 返回参数：无
* 说    明： 
****************************************************************************/
void uart3SendChars(u8 *str, u16 strlen)
{ 
	u16 k= 0 ; 
   	do 
   	{ 
   		uart3SendChar(*(str + k)); k++; 
   	}while (k < strlen); //循环发送,直到发送完毕  
} 

__align(8) u8  send_str[UART3_TRS_NUM];
void u3_printf(char* fmt,...)  
{  
	int cnt = 0;
	int len = 0;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)send_str,fmt,ap);//将ap按照fmt格式写入串口3缓存区
	va_end(ap);//结束变参获取 
	len = strlen((const char*)send_str);
	while (len--)
	{
		while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == 0);
		USART_SendData(USART3,send_str[cnt++]);
	}                        
}

//串口1中断服务程序
void USART3_IRQHandler(void)  
{
	u8 rec_data;
	int len = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断 
	{
		rec_data =(u8)USART_ReceiveData(USART3);         //(USART1->DR) 读取接收到的数据
        if(rec_data=='S')		  	                         //如果是S，表示是命令信息的起始位
		{
			uart_byte_count=0x01; 
		}

		else if(rec_data=='E')		                         //如果E，表示是命令信息传送的结束位
		{
			if(strcmp("open_led1",(char *)receive_str)==0)         LED0=0;	   //点亮LED1
			else if(strcmp("close_led1",(char *)receive_str)==0)   LED0=1;	   //关灭LED1
			else if(strcmp("open_led2",(char *)receive_str)==0)    LED1=0;	   //关灭LED1
			else if(strcmp("close_led2",(char *)receive_str)==0)   LED1=1;	   //关灭LED1
			else if(strcmp("open_led3",(char *)receive_str)==0)    LED2=0;	   //关灭LED1
			else if(strcmp("close_led3",(char *)receive_str)==0)   LED2=1;	   //关灭LED1
			else if(strcmp("open_beep",(char *)receive_str)==0)    BEEP=1; 	   //蜂鸣器响
			else if(strcmp("close_beep",(char *)receive_str)==0)   BEEP=0; 	   //蜂鸣器不响
			else if(strcmp("test",(char *)receive_str)==0)
			{
				uart3SendChars("correct_OKcorrect_OKcorrect_OK",30);
			}
			else if(strcmp("Data",(char *)receive_str)==0)
			{
				//............test
				len = strlen((const char*)package_data);

				//串口1可以发
				uart1SendChars(package_data,len);
				//uart3SendChars(package_data,len);
			}
			else if(strcmp("send_acc",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",a[0],"m/s2, ");
				u3_printf("%.3f%s",a[1],"m/s2, ");
				u3_printf("%.3f%s",a[2],"m/s2;");
				USART_SendData(USART3,0x1a);
				*/
				sprintf(send_str, "%.3f%s",a[0],"m/s2");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
				sprintf(send_str, "%.3f%s",a[1],"m/s2");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
				sprintf(send_str, "%.3f%s",a[2],"m/s2");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("send_pal",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",w[0],"度/s, ");
				u3_printf("%.3f%s",w[1],"度/s, ");
				u3_printf("%.3f%s",w[2],"度/s;");
			    USART_SendData(USART3,0x1a);
			    */
			    sprintf(send_str, "%.3f%s",w[0],"d/s");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
				sprintf(send_str, "%.3f%s",w[1],"d/s");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
				sprintf(send_str, "%.3f%s",w[2],"d/s");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("send_ang",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",angle[0],"度, ");
				u3_printf("%.3f%s",angle[1],"度, ");
				u3_printf("%.3f%s",angle[2],"度;");
			    USART_SendData(USART3,0x1a);
			    */
			    sprintf(send_str, "%.3f%s",angle[0],"d");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
				sprintf(send_str, "%.3f%s",angle[1],"d");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
				sprintf(send_str, "%.3f%s",angle[2],"d");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("send_tem",(char *)receive_str)==0)
			{
				LED2=!LED2;//灯亮
				/*
				u3_printf("%.3f%s",T,"度;");
			    USART_SendData(USART3,0x1a);
			    */
			    sprintf(send_str, "%.3f%s",T,"C");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("send_dis1",(char *)receive_str)==0)
			{
				LED2=!LED2;//灯亮
				
			  sprintf(send_str, "%.3f%s",dis[0],"cm");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("send_voi",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",T,"度;");
			    USART_SendData(USART3,0x1a);
			    */
				voice = (float)Get_Adc(ADC_Channel_4) / 4096.0f * 430.0 / 5.0;
			  sprintf(send_str, "%.3f%s",voice,"dB");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("app_mode1",(char *)receive_str)==0)    
		    {
				APP_mode=0; 
		        LCD_DisplayString(20,130,16,"APP_mode: 0  ");
            	LCD_DisplayString(20,150,16,"Wait APP Control");
				LCD_Fill_onecolor(0,170,239,319,WHITE);
		    }//APP为状态控制区
			else if(strcmp("app_mode2",(char *)receive_str)==0)    
		    {
				APP_mode=1;
				LCD_DisplayString(20,130,16,"APP_mode: 1  ");
                LCD_DisplayString(20,150,16,"Receive and send");
				LCD_Fill_onecolor(0,170,239,319,WHITE);
		    }//APP为接收发送区
			
			else if(receive_str[0]=='T')   //时间校准
		    {
				RTC_DateStruct.RTC_Year= (receive_str[3]-'0')*10 + (receive_str[4]-'0');						
				RTC_DateStruct.RTC_Month= (receive_str[5]-'0')*10 + (receive_str[6]-'0');							
				RTC_DateStruct.RTC_Date= (receive_str[7]-'0')*10 + (receive_str[8]-'0');							
				RTC_TimeStruct.RTC_Hours= (receive_str[9]-'0')*10 + (receive_str[10]-'0');						
				RTC_TimeStruct.RTC_Minutes= (receive_str[11]-'0')*10 + (receive_str[12]-'0');					
				RTC_TimeStruct.RTC_Seconds= (receive_str[13]-'0')*10 + (receive_str[14]-'0');		

				RTC_SetTimes(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
				delay_ms(100);
				uart3SendChars("correct_OK",10);
			 }
		    if(APP_mode==1)   //APP为接收发送区时，显示APP发来的数据
			{
			    LCD_DisplayString(30,190,16,receive_str);	 //可以根据自己发送的数据 写相应的动作 按前面的编写方法
			}
				
			for(uart_byte_count=0;uart_byte_count<32;uart_byte_count++)receive_str[uart_byte_count]=0x00;
			uart_byte_count=0;    
		}				  
		else if((uart_byte_count>0)&&(uart_byte_count<=UART3_REC_NUM))
		{
		   receive_str[uart_byte_count-1]=rec_data;
		   uart_byte_count++;
		}                		 
    } 
} 

