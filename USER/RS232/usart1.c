#include "usart1.h"
#include "string.h"
#include "stdlib.h"  
#include "led.h" 
#include "beep.h" 
#include "usart3_ble.h"
#include "Sensor.h"
#include "rtc.h"
#include "mpu6050.h"
#include "gsm.h"
#include "adc.h"
#include "rs485.h"
#include "inf.h"
#include "malloc.h"
#include "24c02.h"  
#include "w25qxx.h"
//#include <stdio.h>

/*********************************************************************************
**********************MCUÆôÃ÷ STM32F407Ó¦ÓÃ¿ª·¢°å(ÔöÇ¿°æ)*************************
**********************************************************************************
* ÎÄ¼þÃû³Æ: usart1.c                                                             *
* ÎÄ¼þ¼òÊö£ºUSART1Ê¹ÓÃ                                                           *
* ´´½¨ÈÕÆÚ£º2015.03.06                                                           *
* °æ    ±¾£ºV1.0                                                                 *
* ×÷    Õß£ºClever                                                               *
* Ëµ    Ã÷£ºÀûÓÃ´®¿Úµ÷ÊÔÖúÊÖ¾­¹ýUSART1¿ØÖÆLEDÁÁÃðÓë·äÃùÆ÷Ïì±Õ                    * 
**********************************************************************************
*********************************************************************************/	

u8 receive_str1[USART1_REC_NUM];     //½ÓÊÕ»º´æÊý×é,×î´óUSART_REC_LEN¸ö×Ö½Ú 
u8 package_data[USART1_REC_MASTER_NUM];
u8 uart1_byte_count=0;
u8 send_str1[UART1_TRS_NUM];

u8 uart1_data_count=0;
u8 full_flag = 0;

Sensor* result_slave;
Sensor_master* result_master;

/****************************************************************************
* Ãû    ³Æ: void uart1_init(u32 bound)
* ¹¦    ÄÜ£ºUSART1³õÊ¼»¯
* Èë¿Ú²ÎÊý£ºbound£º²¨ÌØÂÊ
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 
****************************************************************************/
void uart1_init(u32 bound)
{   //GPIO¶Ë¿ÚÉèÖÃ
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //Ê¹ÄÜGPIOAÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//Ê¹ÄÜUSART1Ê±ÖÓ 
	//´®¿Ú1¶ÔÓ¦Òý½Å¸´ÓÃÓ³Éä
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  //GPIOA9¸´ÓÃÎªUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10¸´ÓÃÎªUSART1
	//USART1¶Ë¿ÚÅäÖÃ
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9ÓëGPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //¸´ÓÃ¹¦ÄÜ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ËÙ¶È50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍì¸´ÓÃÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //ÉÏÀ­
	GPIO_Init(GPIOA,&GPIO_InitStructure);          //³õÊ¼»¯PA9£¬PA10
   //USART1 ³õÊ¼»¯ÉèÖÃ
	USART_InitStructure.USART_BaudRate = bound;//²¨ÌØÂÊÉèÖÃ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //Ò»¸öÍ£Ö¹Î»
	USART_InitStructure.USART_Parity = USART_Parity_No;//ÎÞÆæÅ¼Ð£ÑéÎ»
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½
  USART_Init(USART1, &USART_InitStructure); //³õÊ¼»¯´®¿Ú1	
  USART_Cmd(USART1, ENABLE);  //Ê¹ÄÜ´®¿Ú1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);         //¿ªÆôÏà¹ØÖÐ¶Ï
	//Usart1 NVIC ÅäÖÃ
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;      //´®¿Ú1ÖÐ¶ÏÍ¨µÀ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		   //×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	  //¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷¡¢
}

//´®¿Ú1·¢ËÍÒ»¸ö×Ö·û
void uart1SendChar(u8 ch)
{      
	while((USART1->SR&0x40)==0);  
    USART1->DR = (u8) ch;      
}
/****************************************************************************
* Ãû    ³Æ: void uart1SendChars(u8 *str, u16 strlen)
* ¹¦    ÄÜ£º´®¿Ú1·¢ËÍÒ»×Ö·û´®
* Èë¿Ú²ÎÊý£º*str£º·¢ËÍµÄ×Ö·û´®
            strlen£º×Ö·û´®³¤¶È
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 
****************************************************************************/
void uart1SendChars(u8 *str, u16 strlen)
{ 
	  u16 k= 0 ; 
   do { uart1SendChar(*(str + k)); k++; }   //Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
    while (k < strlen); 
} 

//´®¿Ú1ÖÐ¶Ï·þÎñ³ÌÐò
void USART1_IRQHandler(void)  
{
	u8 rec_data;
	int len = 0;
	int i = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //½ÓÊÕÖÐ¶Ï 
		{
			LED1 = !LED1;
				rec_data =(u8)USART_ReceiveData(USART1);         //(USART1->DR) ¶ÁÈ¡½ÓÊÕµ½µÄÊý¾Ý
        		if(rec_data=='{')		  	                         //Èç¹ûÊÇS£¬±íÊ¾ÊÇÃüÁîÐÅÏ¢µÄÆðÊ¼Î»
				{
					receive_str1[uart1_byte_count]=rec_data;
					uart1_byte_count=0x01; 
					
				}

				else if(rec_data=='}')		                         //Èç¹ûE£¬±íÊ¾ÊÇÃüÁîÐÅÏ¢´«ËÍµÄ½áÊøÎ»
				{
					// if(strcmp("Light_led1",(char *)receive_str1)==0)        LED1=0;	//µãÁÁLED1
					// else if(strcmp("Close_led1",(char *)receive_str1)==0)   LED1=1;	//¹ØÃðLED1
					// else if(strcmp("Open_beep",(char *)receive_str1)==0)    BEEP=1; 	//·äÃùÆ÷Ïì
					// else if(strcmp("Close_beep",(char *)receive_str1)==0)   BEEP=0; 	//·äÃùÆ÷²»Ïì
					// else if(strcmp("send_dis1",(char *)receive_str1)==0)
					// {//²âÊÔÊ§°Ü£¬usart3_ble¶¨ÒåµÄextern±äÁ¿ÔÚmainÖÐÐÞ¸Äºóusart_bleÄÜÓÃ£¬´ËÄÚ²»ÄÜÓÃ
					// 	sprintf(send_str, "%.3f%s",dis1,"cm");
					// 	len = strlen((const char*)send_str);
					// 	uart1SendChars(send_str,len);
					// }
					
					//给slave返回OK
					LED2 = !LED2;
					char tmp[30] = "";
					tmp[0]=0x4f;
					tmp[1]=0x4b;
					tmp[2]=0x0d;
					tmp[3]=0x0a;
				    len = strlen((const char*)tmp);
				    uart1SendChars(tmp,len);
					
					//24c02存储记录条数 AT24C02_WriteByte  AT24C02_ReadByte
					uart1_data_count = AT24C02_ReadByte(0);
					if(uart1_data_count >= 255) 
					{
						uart1_data_count = 0;  //可以存255,下一次再判断
						full_flag = 1;		   //表示之前存过255条
					}
					uart1_data_count ++;
					AT24C02_WriteByte(0, uart1_data_count);
					uart1SendChar(uart1_data_count);
					

					//package
					receive_str1[uart1_byte_count]=rec_data;
				    uart1_byte_count++;

				    result_slave = Sensor_Create();
				    result_master = Sensor_master_Create();
				    result_slave = get_Struct(receive_str1);

				    //u8 i = 0;
	
					result_master->distance = result_slave->distance;
					result_master->humidity = result_slave->humidity;
					result_master->light = result_slave->light;
					result_master->temperature = result_slave->temperature;
					for(i = 0; i < 3; i ++)
					{
						result_master->voltage[i] = result_slave->voltage[i];
					}
					for(i = 0; i < 3; i ++)
					{
						result_master->Amper[i] = result_slave->Amper[i];
					}

					result_master->voice = voice;
					for(i = 0; i < 3; i ++)
					{
						result_master->acc[i] = a[i];
					}
					for(i = 0; i < 3; i ++)
					{
						result_master->pal[i] = w[i];
					}
					for(i = 0; i < 3; i ++)
					{
						result_master->angle[i] = angle[i];
					}
					for(i = 0; i < 3; i ++)
					{
						result_master->dis[i] = dis[i];
					}

					//char*  Óë char* 
					//strcpy(result_master->date, result_slave->date);

					//result_master->date = result_slave->date;
					RTC_GetTimes(RTC_Format_BIN);
					char temp[30];
					sprintf((char*)temp,"20%02d/%02d/%02d %02d:%02d:%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
					//sprintf((char*)temp,"20%02d/%02d/%02d %02d:%02d:%02d",22,22,22,22,22,22);

					// test
					//char * temp = result_slave->date;
					len = strlen((const char*)temp);
				    //uart1SendChars(temp,len);

					//char[]给char*赋值
				    result_master->date = (char*)mymalloc(SRAMIN,30*sizeof(char));
				    strncpy(result_master->date, temp, len);
				    // for(i = 0; i < len; i++)
				    // {
				    // 	*(result_master->date++) = temp[i];
				    // }
				    //len = strlen((const char*)result_master->date);
				    //uart1SendChars(result_master->date,len);

					//char* Óë char[],²»ÄÜÖ±½Ó¸³Öµ£¬³õÊ¼»¯³ýÍâ
					strcpy(package_data, Sensor_master_to_String(result_master)); 


					//W25Q128 存储2Bytes长度 + package; u8*<---->u16
					u16 lenp = strlen((const char*)package_data);
				    W25QXX_SectorWrite((u8*)lenp,(uart1_data_count-1)*4096,2);
				    W25QXX_SectorWrite((u8*)package_data,(uart1_data_count-1)*4096+2,lenp);

				    //test
				    //Áªµ÷µÄÊ±ºò·¢OKodoa
				    // len = strlen((const char*)receive_str1);
				    // uart1SendChars(receive_str1,len);

				    // len = strlen((const char*)package_data);
				    // uart1SendChars(package_data,len);

				    //char tmp[30] = "OK\r\n";
				    //len = strlen((const char*)tmp);
				    //uart1SendChars(tmp,len);

					for(i=0;i<uart1_byte_count;i++)receive_str1[i]=0x00;
					uart1_byte_count=0;    
				}				  
				else if((uart1_byte_count>0)&&(uart1_byte_count<=USART1_REC_NUM))
				{
				   receive_str1[uart1_byte_count]=rec_data;
				   uart1_byte_count++;
				}

   } 
} 

