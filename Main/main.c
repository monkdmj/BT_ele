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
**********************MCU���� STM32F407Ӧ�ÿ�����(�����)*************************
**********************************************************************************
* �ļ�����: ����ģ��ʹ������                                                     *
* �ļ�������                                                                     *
* �������ڣ�2015.11.06                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ���������Ѿ����úã�������Ϊ9600��ֻҪ��Ӧ�Ŀ��ƴ���(���ﴮ��4)���úò�  *
            ����Ϊ9600�Ϳ�����ʹ�ô���һ��ʹ������ģ��ʵ������͸��               * 
* �Ա����̣�https://shop125046348.taobao.com                                     *
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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����ϵͳ�ж����ȼ�����2
	delay_init();		      //��ʱ��ʼ�� 
	
	KEY_Init();
	LED_Init();		  		  //��ʼ����LED 
	BEEP_Init();          //��������ʼ��
  LCD_Init();           //��ʼ��LCD FSMC�ӿں���ʾ����
	RTC_InitConfig();
  LTE_uart3_init(9600);	    //���ڳ�ʼ��������Ϊ9600

	GSM_Init();
	USART6_Config();

  uart1_init(115200);

	ADC1_Init();
	
	ADC2_Init();
	ADC3_Init();
	ADC4_Init();
	ADC5_Init();

	RS485_Init(9600);		//��ʼ��RS485����2

	AT24C02_Init();			//AT24C02��ʼ��
	/*
	BRUSH_COLOR=RED;      //���û�����ɫΪ��ɫ
	
	LCD_DisplayString(20,10,24,"Illuminati STM32F4");	
  LCD_DisplayString(20,40,16,"Author:Clever");
	LCD_DisplayString(30,80,24,"LTE TEST");
	
	BRUSH_COLOR=BLUE; 
	*/
	while(LTE_connect==0)     // ����4.0ģ���������ֻ����
	{
 	  //LCD_DisplayString(20,110,16,"Bluetooth not connected");
		LED2=0;
		delay_ms(500);
		LED2=1;
   	//LCD_DisplayString(20,110,16,"                       ");
		//delay_ms(500);
	}
	
	//LCD_DisplayString(20,110,16,"Bluetooth connected succeed");	  //ģ�����������ֻ�
	
  //LCD_DisplayString(20,130,16,"APP_mode: 0  ");
	//LCD_DisplayString(20,150,16,"Wait APP Control ");
	
	RTC_GetTimes(RTC_Format_BIN);
	//Time_Display();
	
	while(1)
	{
		if(APP_mode==0)
		 {
				if(t>500)   //2500ms����ʱ����ɼ�һ���¶�ֵ
				{ 									
          //RTC_GetTimes(RTC_Format_BIN);
          //Time_Display();	
					
					/*
					temperature=DS18B20_Get_Temp();	
					temperature=934;
					LCD_DisplayString(30,190,16,"T:   . C");
					if(temperature<0)
					{
						LCD_DisplayChar(30+16,190,'-',16);			//��ʾ����
						temperature=-temperature;					      //תΪ����
						tempbuf[0]='-';
					}
					else 
					{
						LCD_DisplayChar(30+16,190,' ',16);			//ȥ������
						tempbuf[0]=' ';
					}
					LCD_DisplayNum(30+24,190,temperature/10,2,16,0);	//��ʾ��������	    
					LCD_DisplayNum(30+48,190,temperature%10,1,16,0);	//��ʾС������
					
					tempbuf[1]=temperature/100%10+'0';
					tempbuf[2]=temperature/10%10+'0';
					tempbuf[3]='.';
					tempbuf[4]=temperature%10+'0';
					
					uart3SendChars(tempbuf,5);
					*/
					//LED1 = !LED1;
					//�̽�PA2,PA3��usart2_irq�е�ƣ�����USART2���óɹ�
					//USART_SendData(USART2,0x1a);
					//USART_SendData(UART4,0x1a);//SUCCES
					
					voice = (float)Get_Adc(ADC_Channel_4) / 4096.0f * 430.0 / 5.0;
					dis[0] = (float)Get_Adc2_Average(ADC_Channel_5,5);
					
					//����1��ӡ����
					//��ʱ�ر�
					// sprintf(send_rs232, "%.3f%s",voice,"db");
					// len = strlen((const char*)send_rs232);
					// uart1SendChars(send_rs232,len);
					
					// sprintf(send_rs232, "%.3f%s",dis[0],"cm");
					// len = strlen((const char*)send_rs232);
					// uart1SendChars(send_rs232,len);
		
					//char*��������
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

