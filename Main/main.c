#include "usart3_ble.h"
#include "led.h"
#include "beep.h"
#include "lcd.h"
#include "key.h"
#include "ds18b20.h"
#include "rtc.h"
#include "mpu6050.h"
#include "gsm.h"
#include "adc.h"
#include "rs485.h"
#include "inf.h"
#include "usart1.h"
#include "24c02.h" 
// #include <stdio.h>

/*********************************************************************************
**********************MCU启明 STM32F407应用开发板(高配版)*************************
**********************************************************************************
* 文件名称: 蓝牙模块使用例程                                                     *
* 文件简述：                                                                     *
* 创建日期：2015.11.06                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：蓝牙已经设置好，波特率为9600，只要相应的控制串口(这里串口4)设置好波  *
            特率为9600就可以像使用串口一样使用蓝牙模块实现数据透传               * 
* 淘宝店铺：https://shop125046348.taobao.com                                     *
**********************************************************************************
*********************************************************************************/

u8 const *weekdate[7]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
float voice = 0.0f;
float dis[4] = {0.0f, 0.0f, 0.0f, 0.0f};



void Time_Display()
{
 LCD_DisplayString(30,220,16,"Date:20  -  -  ");
 LCD_DisplayNum(86,220,RTC_DateStruct.RTC_Year,2,16,1);
 LCD_DisplayNum(110,220,RTC_DateStruct.RTC_Month,2,16,1);
 LCD_DisplayNum(134,220,RTC_DateStruct.RTC_Date,2,16,1);
 LCD_DisplayString(30,240,16,"Time:  :  :  ");
 LCD_DisplayNum(70,240,RTC_TimeStruct.RTC_Hours,2,16,1);	
 LCD_DisplayNum(94,240,RTC_TimeStruct.RTC_Minutes,2,16,1);
 LCD_DisplayNum(118,240,RTC_TimeStruct.RTC_Seconds,2,16,1);
 LCD_DisplayString(30,260,16,"Week:        ");
 LCD_DisplayString(70,260,16,(u8 *)weekdate[RTC_DateStruct.RTC_WeekDay-1]);
}

int main(void)
{   
	int t=0;	
	short temperature;
  u8  tempbuf[9]={0}; 
	u8 send_rs232[100];
	int len = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
	delay_init();		      //延时初始化 
	
	KEY_Init();
	LED_Init();		  		  //初始化与LED 
	BEEP_Init();          //蜂鸣器初始化
  LCD_Init();           //初始化LCD FSMC接口和显示驱动
	RTC_InitConfig();
  LTE_uart3_init(9600);	    //串口初始化波特率为9600

	GSM_Init();
	USART6_Config();

  uart1_init(115200);

	ADC1_Init();
	
	ADC2_Init();
	ADC3_Init();
	ADC4_Init();
	ADC5_Init();

	RS485_Init(9600);		//初始化RS485串口2

	AT24C02_Init();			//AT24C02初始化
	/*
	BRUSH_COLOR=RED;      //设置画笔颜色为红色
	
	LCD_DisplayString(20,10,24,"Illuminati STM32F4");	
  LCD_DisplayString(20,40,16,"Author:Clever");
	LCD_DisplayString(30,80,24,"LTE TEST");
	
	BRUSH_COLOR=BLUE; 
	*/
	while(LTE_connect==0)     // 蓝牙4.0模块连接上手机与否
	{
 	  //LCD_DisplayString(20,110,16,"Bluetooth not connected");
		LED2=0;
		delay_ms(500);
		LED2=1;
   	//LCD_DisplayString(20,110,16,"                       ");
		//delay_ms(500);
	}
	
	//LCD_DisplayString(20,110,16,"Bluetooth connected succeed");	  //模块已连接上手机
	
  //LCD_DisplayString(20,130,16,"APP_mode: 0  ");
	//LCD_DisplayString(20,150,16,"Wait APP Control ");
	
	RTC_GetTimes(RTC_Format_BIN);
	//Time_Display();
	
	while(1)
	{
		if(APP_mode==0)
		 {
				if(t>500)   //2500ms更新时间与采集一次温度值
				{ 									
          //RTC_GetTimes(RTC_Format_BIN);
          //Time_Display();	
					
					/*
					temperature=DS18B20_Get_Temp();	
					temperature=934;
					LCD_DisplayString(30,190,16,"T:   . C");
					if(temperature<0)
					{
						LCD_DisplayChar(30+16,190,'-',16);			//显示负号
						temperature=-temperature;					      //转为正数
						tempbuf[0]='-';
					}
					else 
					{
						LCD_DisplayChar(30+16,190,' ',16);			//去掉负号
						tempbuf[0]=' ';
					}
					LCD_DisplayNum(30+24,190,temperature/10,2,16,0);	//显示正数部分	    
					LCD_DisplayNum(30+48,190,temperature%10,1,16,0);	//显示小数部分
					
					tempbuf[1]=temperature/100%10+'0';
					tempbuf[2]=temperature/10%10+'0';
					tempbuf[3]='.';
					tempbuf[4]=temperature%10+'0';
					
					uart3SendChars(tempbuf,5);
					*/
					//LED1 = !LED1;
					//短接PA2,PA3，usart2_irq中点灯，测试USART2配置成功
					//USART_SendData(USART2,0x1a);
					//USART_SendData(UART4,0x1a);//SUCCES
					
					voice = (float)Get_Adc(ADC_Channel_4) / 4096.0f * 430.0 / 5.0;
					dis[0] = (float)Get_Adc2_Average(ADC_Channel_5,5);
					
					//串口1打印测试
					//暂时关闭
					// sprintf(send_rs232, "%.3f%s",voice,"db");
					// len = strlen((const char*)send_rs232);
					// uart1SendChars(send_rs232,len);
					
					// sprintf(send_rs232, "%.3f%s",dis[0],"cm");
					// len = strlen((const char*)send_rs232);
					// uart1SendChars(send_rs232,len);
		
					//char*就是乱码
					// char *temp;
					// sprintf(temp,"20%02d/%02d/%02d %02d:%02d:%02d",22,22,22,22,22,22);
					// len = strlen((const char*)temp);
				 //    uart1SendChars(temp,len);
					/*
					GSM_SendAccData(a[0],a[1],a[2]);
					GSM_SendPalData(w[0],w[1],w[2]);
					GSM_SendAngData(angle[0],angle[1],angle[2]);
					u4_printf("%s\r\n","AT+CIPSEND");
	        delay_ms(200);
					u4_printf("%.3f%s",voice,"dB");
					u4_printf("%.3f%s",T,"C");
					USART_SendData(UART4,0x1a);
		      delay_ms(200);
					*/
					
					t = 0;
				}		
		 }
		
		else if(APP_mode==1)
		 {
				key_scan(0);
				
				if(keyup_data==KEY0_DATA)
					{
					 uart3SendChars("LTE TEST",9);
					}
				if(keyup_data==KEY1_DATA)
					{
					 uart3SendChars(" TEST",9);
					}
		 }
		 delay_ms(5);
		 t++;
	}
}

