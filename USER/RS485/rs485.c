#include "rs485.h"	
#include "string.h"
#include "stdlib.h"  
#include "led.h" 
#include "beep.h"
	 
/*********************************************************************************
**********************MCUÆôÃ÷ STM32F407Ó¦ÓÃ¿ª·¢°å(¸ßÅä°æ)*************************
**********************************************************************************
* ÎÄ¼þÃû³Æ: rs485.c                                                              *
* ÎÄ¼þ¼òÊö£ºrs485³õÊ¼»¯                                                          *
* ´´½¨ÈÕÆÚ£º2015.10.03                                                           *
* °æ    ±¾£ºV1.0                                                                 *
* ×÷    Õß£ºClever                                                               *
* Ëµ    Ã÷£º                                                                     * 
**********************************************************************************
*********************************************************************************/


  	  
//½ÓÊÕ»º´æÇø 	
u8 RS485_receive_str[128];   //½ÓÊÕ»º³å,×î´ó128¸ö×Ö½Ú.
u8 uart2_byte_count=0;        //½ÓÊÕµ½µÄÊý¾Ý³¤¶È

										 
//³õÊ¼»¯IO ´®¿Ú2   bound:²¨ÌØÂÊ	
void RS485_Init(u32 bound)
{  	 
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //Ê¹ÄÜGPIOAÊ±ÖÓ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//Ê¹ÄÜUSART2Ê±ÖÓ
	
  //´®¿Ú2Òý½Å¸´ÓÃÓ³Éä
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2¸´ÓÃÎªUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3¸´ÓÃÎªUSART2
	
	//USART2    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2ÓëGPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//¸´ÓÃ¹¦ÄÜ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//ËÙ¶È100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍì¸´ÓÃÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //ÉÏÀ­
	GPIO_Init(GPIOA,&GPIO_InitStructure); //³õÊ¼»¯PA2£¬PA3
	
	//PG8ÍÆÍìÊä³ö£¬485Ä£Ê½¿ØÖÆ  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //GPIOG6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//Êä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//ËÙ¶È100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //ÉÏÀ­
	GPIO_Init(GPIOG,&GPIO_InitStructure); //³õÊ¼»¯PG8
	
   //USART2 ³õÊ¼»¯ÉèÖÃ
	USART_InitStructure.USART_BaudRate = bound;//²¨ÌØÂÊÉèÖÃ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure.USART_Parity = USART_Parity_No;//ÎÞÆæÅ¼Ð£ÑéÎ»
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½
  USART_Init(USART2, &USART_InitStructure); //³õÊ¼»¯´®¿Ú2
	
  USART_Cmd(USART2, ENABLE);  //Ê¹ÄÜ´®¿Ú 2
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//¿ªÆô½ÓÊÜÖÐ¶Ï

	//Usart2 NVIC ÅäÖÃ
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷¡¢
	
	RS485_TX_EN=0;				//³õÊ¼»¯Ä¬ÈÏÎª½ÓÊÕÄ£Ê½	
}

//´®¿Ú2½ÓÊÕÖÐ¶Ï·þÎñº¯Êý
void USART2_IRQHandler(void)
{
	u8 rec_data;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//½ÓÊÕµ½Êý¾Ý
	{	 	
				rec_data =(u8)USART_ReceiveData(USART2);         //(USART2->DR) ¶ÁÈ¡½ÓÊÕµ½µÄÊý¾Ý
        if(rec_data=='S')		  	                         //Èç¹ûÊÇS£¬±íÊ¾ÊÇÃüÁîÐÅÏ¢µÄÆðÊ¼Î»
				{
					uart2_byte_count=0x01; 
				}

			else if(rec_data=='E')		                         //Èç¹ûE£¬±íÊ¾ÊÇÃüÁîÐÅÏ¢´«ËÍµÄ½áÊøÎ»
				{
					if(strcmp("Light_led1",(char *)RS485_receive_str)==0)        LED1=0;	//µãÁÁLED1
					else if(strcmp("Close_led1",(char *)RS485_receive_str)==0)   LED1=1;	//¹ØÃðLED1
					else if(strcmp("Open_beep",(char *)RS485_receive_str)==0)    BEEP=1; 	//·äÃùÆ÷Ïì
					else if(strcmp("Close_beep",(char *)RS485_receive_str)==0)   BEEP=0; 	//·äÃùÆ÷²»Ïì
					
					for(uart2_byte_count=0;uart2_byte_count<32;uart2_byte_count++) RS485_receive_str[uart2_byte_count]=0x00;
					uart2_byte_count=0;    
				}				  
			else if((uart2_byte_count>0)&&(uart2_byte_count<=128))
				{
				   RS485_receive_str[uart2_byte_count-1]=rec_data;
				   uart2_byte_count++;
				}
	}  											 
} 


/****************************************************************************
* Ãû    ³Æ: void RS485_Send_Data(u8 *buf,u8 len)
* ¹¦    ÄÜ£ºRS485·¢ËÍlen¸ö×Ö½Ú
* Èë¿Ú²ÎÊý£ºbuf:·¢ËÍÇøÊ×µØÖ·
            len:·¢ËÍµÄ×Ö½ÚÊý 
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º(ÎªÁËºÍ±¾´úÂëµÄ½ÓÊÕÆ¥Åä,ÕâÀï½¨ÒéÊý¾Ý³¤¶È²»Òª³¬¹ý128¸ö×Ö½Ú)       
****************************************************************************/	
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;			    //ÉèÖÃÎª·¢ËÍÄ£Ê½
  	for(t=0;t<len;t++)		//Ñ­»··¢ËÍÊý¾Ý
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //µÈ´ý·¢ËÍ½áÊø		
    USART_SendData(USART2,buf[t]); //·¢ËÍÊý¾Ý
	}	 
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);   //µÈ´ý·¢ËÍ½áÊø		
	uart2_byte_count=0;	  
	RS485_TX_EN=0;				//·¢ËÍÍêÉèÖÃÎª½ÓÊÕÄ£Ê½	
}

/****************************************************************************
* Ãû    ³Æ: void RS485_Receive_Data(u8 *buf,u8 *len)
* ¹¦    ÄÜ£ºRS485²éÑ¯½ÓÊÕµ½µÄÊý¾Ý
* Èë¿Ú²ÎÊý£ºbuf:½ÓÊÕ»º´æÊ×µØÖ·
            len:¶Áµ½µÄÊý¾Ý³¤¶È 
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º      
****************************************************************************/
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=uart2_byte_count;
	u8 i=0;
	*len=0;				   //Ä¬ÈÏÎª0
	delay_ms(10);		 //µÈ´ý10ms,Á¬Ðø³¬¹ý10msÃ»ÓÐ½ÓÊÕµ½Ò»¸öÊý¾Ý,ÔòÈÏÎª½ÓÊÕ½áÊø
	
	if(rxlen==uart2_byte_count&&rxlen) //½ÓÊÕµ½ÁËÊý¾Ý,ÇÒ½ÓÊÕÍê³ÉÁË
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_receive_str[i];	
		}		
		*len=uart2_byte_count;	//¼ÇÂ¼±¾´ÎÊý¾Ý³¤¶È
		uart2_byte_count=0;		  //ÇåÁã
	}
}





