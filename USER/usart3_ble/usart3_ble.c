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
#include "w25qxx.h"
#include "24c02.h" 
#include "timer.h"
#include "rs485.h"	

/*********************************************************************************
*********************MCUÆôÃ÷ STM32F407Ó¦ÓÃ¿ª·¢°å(ÔöÇ¿°æ)**************************
**********************************************************************************
* ÎÄ¼þÃû³Æ: lte.c                                                                *
* ÎÄ¼þ¼òÊö£ºÀ¶ÑÀÊ¹ÓÃ                                                             *
* ´´½¨ÈÕÆÚ£º2015.10.06                                                           *
* °æ    ±¾£ºV1.0                                                                 *
* ×÷    Õß£ºClever                                                               *
* Ëµ    Ã÷£ºÀûÓÃÊÖ»úAPP¿ØÖÆ¿ª·¢°å                                                * 
**********************************************************************************
*********************************************************************************/	

u8 receive_str[UART3_REC_NUM];     //½ÓÊÕ»º´æÊý×é,×î´óUSART_REC_LEN¸ö×Ö½Ú 
u8 send_str[UART3_TRS_NUM]; 
u8 uart_byte_count=0;
//u8 APP_mode=0;          //APP¿ØÖÆÄ£Ê½  0£ºÃüÁî¿ØÖÆÇø  1£º½ÓÊÕ·¢ËÍÇø

//float voice = 0.0f;
//float dis1 = 0.0f;
/****************************************************************************
* Ãû    ³Æ: void LTE_uart4_init(u32 bound)
* ¹¦    ÄÜ£ºLTE_uart4³õÊ¼»¯
* Èë¿Ú²ÎÊý£ºbound£º²¨ÌØÂÊ   
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 
****************************************************************************/
void LTE_uart3_init(u32 bound)
{   
/****************************** ´®¿Ú3³õÊ¼»¯*********************************/
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //Ê¹ÄÜGPIOBÊ±ÖÓ 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //Ê¹ÄÜUSART3Ê±ÖÓ 
	//´®¿Ú1¶ÔÓ¦Òý½Å¸´ÓÃÓ³Éä
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);  //GPIOB10¸´ÓÃÎªUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11¸´ÓÃÎªUSART3
	//USART1¶Ë¿ÚÅäÖÃ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10ÓëGPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //¸´ÓÃ¹¦ÄÜ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ËÙ¶È50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍì¸´ÓÃÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //ÉÏÀ­
	GPIO_Init(GPIOB,&GPIO_InitStructure);          //³õÊ¼»¯PB10£¬PB11
   //USART3 ³õÊ¼»¯ÉèÖÃ
	USART_InitStructure.USART_BaudRate = bound;//²¨ÌØÂÊÉèÖÃ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //Ò»¸öÍ£Ö¹Î»
	USART_InitStructure.USART_Parity = USART_Parity_No;//ÎÞÆæÅ¼Ð£ÑéÎ»
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½
    USART_Init(USART3, &USART_InitStructure); //³õÊ¼»¯´®¿Ú1	
    USART_Cmd(USART3, ENABLE);  //Ê¹ÄÜ´®¿Ú1 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);         //¿ªÆôÏà¹ØÖÐ¶Ï
	//Usart1 NVIC ÅäÖÃ
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;      //´®¿Ú1ÖÐ¶ÏÍ¨µÀ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		   //×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	  //¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷¡¢
/****************************** ´®¿Ú3³õÊ¼»¯**********************************/

/******************************* À¶ÑÀIO¿Ú³õÊ¼»¯******************************/
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    //GPIOA7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;             //ÆÕÍ¨ÊäÈëÄ£Ê½
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;       //100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;          
	GPIO_Init(GPIOG,&GPIO_InitStructure);          //³õÊ¼»¯PA9£¬PA10
}

//´®¿Ú1·¢ËÍÒ»¸ö×Ö·û
void uart3SendChar(u8 ch)
{      
	while((USART3->SR&0x40)==0);  
    USART3->DR = (u8) ch;      
}
/****************************************************************************
* Ãû    ³Æ: void uart1SendChars(u8 *str, u16 strlen)
* ¹¦    ÄÜ£º´®¿Ú1·¢ËÍÒ»×Ö·û´®
* Èë¿Ú²ÎÊý£º*str£º·¢ËÍµÄ×Ö·û´®
            strlen£º×Ö·û´®³¤¶È
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 
****************************************************************************/
void uart3SendChars(u8 *str, u16 strlen)
{ 
	u16 k= 0 ; 
   	do 
   	{ 
   		uart3SendChar(*(str + k)); k++; 
   	}while (k < strlen); //Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï  
} 

__align(8) u8  send_str[UART3_TRS_NUM];
void u3_printf(char* fmt,...)  
{  
	int cnt = 0;
	int len = 0;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)send_str,fmt,ap);//½«ap°´ÕÕfmt¸ñÊ½Ð´Èë´®¿Ú3»º´æÇø
	va_end(ap);//½áÊø±ä²Î»ñÈ¡ 
	len = strlen((const char*)send_str);
	while (len--)
	{
		while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == 0);
		USART_SendData(USART3,send_str[cnt++]);
	}                        
}

//´®¿Ú1ÖÐ¶Ï·þÎñ³ÌÐò
void USART3_IRQHandler(void)  
{
	u8 rec_data;
	int len = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //½ÓÊÕÖÐ¶Ï 
	{
		rec_data =(u8)USART_ReceiveData(USART3);         //(USART1->DR) ¶ÁÈ¡½ÓÊÕµ½µÄÊý¾Ý
        if(rec_data=='S')		  	                         //Èç¹ûÊÇS£¬±íÊ¾ÊÇÃüÁîÐÅÏ¢µÄÆðÊ¼Î»
		{
			uart_byte_count=0x01;  //多存一位
		}

		else if(rec_data=='E')		                         //Èç¹ûE£¬±íÊ¾ÊÇÃüÁîÐÅÏ¢´«ËÍµÄ½áÊøÎ»
		{
			//if(strcmp("open_led1",(char *)receive_str)==0)         LED0=0;	   //µãÁÁLED1
			//else if(strcmp("close_led1",(char *)receive_str)==0)   LED0=1;	   //¹ØÃðLED1
			//else if(strcmp("open_led2",(char *)receive_str)==0)    LED1=0;	   //¹ØÃðLED1
			//else if(strcmp("close_led2",(char *)receive_str)==0)   LED1=1;	   //¹ØÃðLED1
			//else if(strcmp("open_led3",(char *)receive_str)==0)    LED2=0;	   //¹ØÃðLED1
			//else if(strcmp("close_led3",(char *)receive_str)==0)   LED2=1;	   //¹ØÃðLED1
			//else if(strcmp("open_beep",(char *)receive_str)==0)    BEEP=1; 	   //·äÃùÆ÷Ïì
			//else if(strcmp("close_beep",(char *)receive_str)==0)   BEEP=0; 	   //·äÃùÆ÷²»Ïì
			if(strcmp("test",(char *)receive_str)==0)
			{
				uart3SendChars("correct_OKcorrect_OKcorrect_OK",30);
			}
			else if(strcmp("status",(char *)receive_str)==0)
			{
				if(RS485_status_success == 1)
				{
					uart3SendChars(RS485_receive_status,status_len);
					//uart1SendChars(RS485_receive_status,status_len);
				}
				else
				{
					uart3SendChars("No status data",14);
				}
				
			}
			else if(strcmp("alarm",(char *)receive_str)==0)
			{
				if(RS485_alarm_success == 1)
				{
					uart3SendChars(RS485_receive_alarm,alarm_len);
					//uart1SendChars(RS485_receive_alarm,alarm_len);
				}
				else
				{
					uart3SendChars("No alarm data",13);
				}
			}
			else if(strcmp("dis",(char *)receive_str)==0)
			{
				//hcrs04_running();
				//sprintf(send_str, "%.3f%s",dis[0],"cm");
				hcrs04_running2();
				sprintf(send_str, "%.3f%s",dis[2],"cm");
				// hcrs04_running1();
				// sprintf(send_str, "%.3f%s",dis[1],"cm");
				// hcrs04_running();
				// sprintf(send_str, "%.3f%s",dis[0],"cm");
				//hcrs04_running2();
				//sprintf(send_str, "%.3f%s",dis[2],"cm");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("Data",(char *)receive_str)==0)
			{
				//............test
				len = strlen((const char*)package_data);

				//´®¿Ú1¿ÉÒÔ·¢
				//uart1SendChars(package_data,len);
				uart3SendChars(package_data,len);
			}
			else if(strcmp("clear",(char *)receive_str)==0)
			{
				//............clear records
				AT24C02_WriteByte(0, 0);
				full_flag = 0;
			}
			// else if(strcmp("set",(char *)receive_str)==0)
			// {
			// 	//............clear records
			// 	AT24C02_WriteByte(0, 0xFA);
			// }
			else if('N'==receive_str[0])
			{
				//............test
				
				//½âÎöÊý×Ö
				u8 i;
				u8 rec_len=0;
				u8 rec_all;
				u16 pac_len;
				//test uart_byte_count
				//uart1SendChar(uart_byte_count);
				//uart1SendChars(receive_str,uart_byte_count-1);

				uart_byte_count -= 1;//多存一位
				if(uart_byte_count == 4)
				{
					rec_len = (receive_str[uart_byte_count-3] - '0') * 100 + (receive_str[uart_byte_count-2] - '0') * 10 + receive_str[uart_byte_count-1] - '0';
				}
				else if(uart_byte_count == 3)
				{
					rec_len = (receive_str[uart_byte_count-2] - '0') * 10 + receive_str[uart_byte_count-1] - '0';
				}
				else if(uart_byte_count == 2)
				{
					//char->number
					rec_len = receive_str[uart_byte_count-1] - '0';
				}

				//test rec_len
				//uart1SendChar(rec_len);

				rec_all = AT24C02_ReadByte(0);
				if((rec_all < rec_len) && (full_flag == 0))
					rec_len = rec_all;
				//在之前存满过的前提下，必须讨论三种情况
				//1，e.g rec_all=123,rec_len = 23;
				//2，e.g rec_all=3,rec_len = 23;
				//3，e.g rec_all=1临界值
				for(i = 1; i <= rec_len; i ++)
				{
					//rec_all:1-255;w25:0-244;
					u8 tmp = (rec_all+255-i)%255;
					u8 pac_tmp_len[2];
					u8 datatemp[USART1_REC_MASTER_NUM];
					W25QXX_Read((u8*)pac_tmp_len,tmp*4096,2);
					pac_len = (pac_tmp_len[0] << 8) + pac_tmp_len[1];
					W25QXX_Read((u8*)datatemp,tmp*4096+2,pac_len);
					//uart1SendChars(datatemp,pac_len);
					uart3SendChars(datatemp,pac_len);
				}

				//·¢ËÍÖ¸¶¨µÄÃüÁîÌõÊý
				//W25QXX_Read(datatemp,0,uart_byte_count);

				//´®¿Ú1¿ÉÒÔ·¢
				//uart1SendChars(package_data,len);
				//uart3SendChars(package_data,len);
			}
			else if(strcmp("acc",(char *)receive_str)==0)
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
			else if(strcmp("pal",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",w[0],"¶È/s, ");
				u3_printf("%.3f%s",w[1],"¶È/s, ");
				u3_printf("%.3f%s",w[2],"¶È/s;");
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
			else if(strcmp("ang",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",angle[0],"¶È, ");
				u3_printf("%.3f%s",angle[1],"¶È, ");
				u3_printf("%.3f%s",angle[2],"¶È;");
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
			else if(strcmp("tem",(char *)receive_str)==0)
			{
				//LED2=!LED2;//µÆÁÁ
				/*
				u3_printf("%.3f%s",T,"¶È;");
			    USART_SendData(USART3,0x1a);
			    */
			  sprintf(send_str, "%.3f%s",T,"C");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("voi",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",T,"¶È;");
			    USART_SendData(USART3,0x1a);
			    */
				voice = (float)Get_Adc(ADC_Channel_4) / 4096.0f * 430.0 / 5.0;
			  sprintf(send_str, "%.3f%s",voice,"dB");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			// else if(strcmp("app_mode1",(char *)receive_str)==0)    
		 //    {
			// 	APP_mode=0; 
		 //        LCD_DisplayString(20,130,16,"APP_mode: 0  ");
   //          	LCD_DisplayString(20,150,16,"Wait APP Control");
			// 	LCD_Fill_onecolor(0,170,239,319,WHITE);
		 //    }//APPÎª×´Ì¬¿ØÖÆÇø
			// else if(strcmp("app_mode2",(char *)receive_str)==0)    
		 //    {
			// 	APP_mode=1;
			// 	LCD_DisplayString(20,130,16,"APP_mode: 1  ");
   //              LCD_DisplayString(20,150,16,"Receive and send");
			// 	LCD_Fill_onecolor(0,170,239,319,WHITE);
		 //    }//APPÎª½ÓÊÕ·¢ËÍÇø
			
			else if(receive_str[0]=='T')   //Ê±¼äÐ£×¼
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
		 //    if(APP_mode==1)   //APPÎª½ÓÊÕ·¢ËÍÇøÊ±£¬ÏÔÊ¾APP·¢À´µÄÊý¾Ý
			// {
			//     //LCD_DisplayString(30,190,16,receive_str);	 //¿ÉÒÔ¸ù¾Ý×Ô¼º·¢ËÍµÄÊý¾Ý Ð´ÏàÓ¦µÄ¶¯×÷ °´Ç°ÃæµÄ±àÐ´·½·¨
			// }
				
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

