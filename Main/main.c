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
#include "w25qxx.h"
#include "timer.h"
#include "rs485.h"	
// #include <stdio.h>

/*********************************************************************************
**********************MCUå¯æ˜Ž STM32F407åº”ç”¨å¼€å‘æ¿(é«˜é…ç‰ˆ)*************************
**********************************************************************************
* æ–‡ä»¶åç§°: è“ç‰™æ¨¡å—ä½¿ç”¨ä¾‹ç¨‹                                                     *
* æ–‡ä»¶ç®€è¿°ï¼š                                                                     *
* åˆ›å»ºæ—¥æœŸï¼š2015.11.06                                                           *
* ç‰ˆ    æœ¬ï¼šV1.0                                                                 *
* ä½œ    è€…ï¼šClever                                                               *
* è¯´    æ˜Žï¼šè“ç‰™å·²ç»è®¾ç½®å¥½ï¼Œæ³¢ç‰¹çŽ‡ä¸º9600ï¼Œåªè¦ç›¸åº”çš„æŽ§åˆ¶ä¸²å£(è¿™é‡Œä¸²å£4)è®¾ç½®å¥½æ³¢  *
            ç‰¹çŽ‡ä¸º9600å°±å¯ä»¥åƒä½¿ç”¨ä¸²å£ä¸€æ ·ä½¿ç”¨è“ç‰™æ¨¡å—å®žçŽ°æ•°æ®é€ä¼                * 
* æ·˜å®åº—é“ºï¼šhttps://shop125046348.taobao.com                                     *
**********************************************************************************
*********************************************************************************/

// u8 const *weekdate[7]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
float voice = 0.0f;
//float dis[4] = {0.0f, 0.0f, 0.0f, 0.0f};



// void Time_Display()
// {
//  LCD_DisplayString(30,220,16,"Date:20  -  -  ");
//  LCD_DisplayNum(86,220,RTC_DateStruct.RTC_Year,2,16,1);
//  LCD_DisplayNum(110,220,RTC_DateStruct.RTC_Month,2,16,1);
//  LCD_DisplayNum(134,220,RTC_DateStruct.RTC_Date,2,16,1);
//  LCD_DisplayString(30,240,16,"Time:  :  :  ");
//  LCD_DisplayNum(70,240,RTC_TimeStruct.RTC_Hours,2,16,1);	
//  LCD_DisplayNum(94,240,RTC_TimeStruct.RTC_Minutes,2,16,1);
//  LCD_DisplayNum(118,240,RTC_TimeStruct.RTC_Seconds,2,16,1);
//  LCD_DisplayString(30,260,16,"Week:        ");
//  LCD_DisplayString(70,260,16,(u8 *)weekdate[RTC_DateStruct.RTC_WeekDay-1]);
// }

int main(void)
{   
	int t=0;	
	// short temperature;
 //  u8  tempbuf[9]={0}; 
	u8 send_rs232[100];
	int len = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //è®¾ç½®ç³»ç»Ÿä¸­æ–­ä¼˜å…ˆçº§åˆ†ç»„2
	delay_init();		      //å»¶æ—¶åˆå§‹åŒ– 
	
	KEY_Init();
	LED_Init();		  		  //åˆå§‹åŒ–ä¸ŽLED 
	BEEP_Init();          //èœ‚é¸£å™¨åˆå§‹åŒ–
  LCD_Init();           //åˆå§‹åŒ–LCD FSMCæŽ¥å£å’Œæ˜¾ç¤ºé©±åŠ¨
	RTC_InitConfig();
  LTE_uart3_init(9600);	    //ä¸²å£åˆå§‹åŒ–æ³¢ç‰¹çŽ‡ä¸º9600

	GSM_Init();
	USART6_Config();

  uart1_init(115200);

	ADC1_Init();
	
	ADC2_Init();
	ADC3_Init();
	ADC4_Init();
	ADC5_Init();

	RS485_Init(9600);		//åˆå§‹åŒ–RS485ä¸²å£2

	AT24C02_Init();			//AT24C02åˆå§‹åŒ–
	W25QXX_Init();			//W25QXXåˆå§‹åŒ–
	HC_SR04_tInit();
	RS485_Init(9600);		//Â³ÃµÃŠÂ¼Â»Â¯RS485Â´Â®Â¿Ãš2
	//AT24C02_WriteByte(0,0); //æ­¤å¤„ä¸èƒ½åˆå§‹åŒ–ï¼Œç›¸å½“äºŽæŽ‰ç”µä¸¢å¤±æ•°æ®
	/*
	BRUSH_COLOR=RED;      //è®¾ç½®ç”»ç¬”é¢œè‰²ä¸ºçº¢è‰²
	
	LCD_DisplayString(20,10,24,"Illuminati STM32F4");	
  LCD_DisplayString(20,40,16,"Author:Clever");
	LCD_DisplayString(30,80,24,"LTE TEST");
	
	BRUSH_COLOR=BLUE; 
	*/
	while(LTE_connect==0)     // è“ç‰™4.0æ¨¡å—è¿žæŽ¥ä¸Šæ‰‹æœºä¸Žå¦
	{
 	  //LCD_DisplayString(20,110,16,"Bluetooth not connected");
		LED2=0;
		delay_ms(500);
		LED2=1;
   	//LCD_DisplayString(20,110,16,"                       ");
		//delay_ms(500);
	}
	
	//LCD_DisplayString(20,110,16,"Bluetooth connected succeed");	  //æ¨¡å—å·²è¿žæŽ¥ä¸Šæ‰‹æœº
	
  //LCD_DisplayString(20,130,16,"APP_mode: 0  ");
	//LCD_DisplayString(20,150,16,"Wait APP Control ");
	
	//RTC_GetTimes(RTC_Format_BIN);
	//Time_Display();
	
	while(1)
	{
		// if(APP_mode==0)
		//  {
				if(t>500)   //2500msæ›´æ–°æ—¶é—´ä¸Žé‡‡é›†ä¸€æ¬¡æ¸©åº¦å€¼
				{ 									
          //RTC_GetTimes(RTC_Format_BIN);
          //Time_Display();	
					
					/*
					temperature=DS18B20_Get_Temp();	
					temperature=934;
					LCD_DisplayString(30,190,16,"T:   . C");
					if(temperature<0)
					{
						LCD_DisplayChar(30+16,190,'-',16);			//æ˜¾ç¤ºè´Ÿå·
						temperature=-temperature;					      //è½¬ä¸ºæ­£æ•°
						tempbuf[0]='-';
					}
					else 
					{
						LCD_DisplayChar(30+16,190,' ',16);			//åŽ»æŽ‰è´Ÿå·
						tempbuf[0]=' ';
					}
					LCD_DisplayNum(30+24,190,temperature/10,2,16,0);	//æ˜¾ç¤ºæ­£æ•°éƒ¨åˆ†	    
					LCD_DisplayNum(30+48,190,temperature%10,1,16,0);	//æ˜¾ç¤ºå°æ•°éƒ¨åˆ†
					
					tempbuf[1]=temperature/100%10+'0';
					tempbuf[2]=temperature/10%10+'0';
					tempbuf[3]='.';
					tempbuf[4]=temperature%10+'0';
					
					uart3SendChars(tempbuf,5);
					*/
					//LED1 = !LED1;
					//çŸ­æŽ¥PA2,PA3ï¼Œusart2_irqä¸­ç‚¹ç¯ï¼Œæµ‹è¯•USART2é…ç½®æˆåŠŸ
					//USART_SendData(USART2,0x1a);
					//USART_SendData(UART4,0x1a);//SUCCES
					
					voice = (float)Get_Adc(ADC_Channel_4) / 4096.0f * 430.0 / 5.0;
					//dis[0] = (float)Get_Adc2_Average(ADC_Channel_5,10) / 4096.0f * 3.3f;
					
					//ä¸²å£1æ‰“å°æµ‹è¯•
					//æš‚æ—¶å…³é—­
					// sprintf(send_rs232, "%.3f%s",voice,"db");
					// len = strlen((const char*)send_rs232);
					// uart1SendChars(send_rs232,len);
					
					//sprintf(send_rs232, "%.3f%s",dis[0],"cm");
					//len = strlen((const char*)send_rs232);
					//uart1SendChars(send_rs232,len);
		
					//char*å°±æ˜¯ä¹±ç 
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
		 // }
		
		// else if(APP_mode==1)
		//  {
		// 		key_scan(0);
				
		// 		if(keyup_data==KEY0_DATA)
		// 			{
		// 			 uart3SendChars("LTE TEST",9);
		// 			}
		// 		if(keyup_data==KEY1_DATA)
		// 			{
		// 			 uart3SendChars(" TEST",9);
		// 			}
		//  }
		 delay_ms(5);
		 t++;
	}
}

