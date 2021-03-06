#include "24c02.h"  				 

/*********************************************************************************
*************************MCUÆôÃ÷ STM32F407ºËÐÄ¿ª·¢°å******************************
**********************************************************************************
* ÎÄ¼þÃû³Æ: 24C02.c                                                              *
* ÎÄ¼þ¼òÊö£º24C02Çý¶¯³ÌÐò                                                        *
* ´´½¨ÈÕÆÚ£º2015.03.09                                                           *
* °æ    ±¾£ºV1.0                                                                 *
* ×÷    Õß£ºClever                                                               *
* Ëµ    Ã÷£ºIO¿ÚÄ£ÄâIICÐ­ÒéÓë24c02¶ÁÐ´²Ù×÷                                       * 
**********************************************************************************
*********************************************************************************/	

//IIC¶ÔÓ¦IO¿ÚµÄ³õÊ¼»¯
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //Ê¹ÄÜGPIOBÊ±ÖÓ
  //GPIOB8,B9³õÊ¼»¯ÉèÖÃ
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          //ÆÕÍ¨Êä³öÄ£Ê½
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //ÍÆÍìÊä³ö
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //ÉÏÀ­
  GPIO_Init(GPIOB, &GPIO_InitStructure);                 //³õÊ¼»¯IO
	IIC_SCL=1;
	IIC_SDAOUT=1;
}
/*******************************************************************************
*************************ÒÔÏÂÎªIO¿ÚÄ£ÄâIICÍ¨ÐÅ**********************************
*******************************************************************************/
//IICÆðÊ¼ÐÅºÅ
void IIC_Start(void)
{
	SDA_OUT();     //ÅäÖÃSDAÏßÎªÊä³ö
	IIC_SDAOUT=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDAOUT=0;
	delay_us(4);
	IIC_SCL=0;     //×¼±¸·¢ËÍ»ò½ÓÊÕÊý¾Ý 
}	  
//IICÍ£Ö¹ÐÅºÅ
void IIC_Stop(void)
{
	SDA_OUT();    //ÅäÖÃSDAÏßÎªÊä³ö
	IIC_SCL=0;
	IIC_SDAOUT=0; 
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDAOUT=1; //·¢ËÍI2C×ÜÏß½áÊøÐÅºÅ
	delay_us(4);							   	
}
/****************************************************************************
* Ãû    ³Æ: u8 MCU_Wait_Ack(void)
* ¹¦    ÄÜ£ºMCUµÈ´ý´ÓÉè±¸Ó¦´ðÐÅºÅµ½À´
* Èë¿Ú²ÎÊý£ºÎÞ
* ·µ»Ø²ÎÊý£º1:½ÓÊÕÓ¦´ðÊ§°Ü  0:½ÓÊÕÓ¦´ð³É¹¦
* Ëµ    Ã÷£º  £B
****************************************************************************/
u8 MCU_Wait_Ack(void)
{
	u8 WaitTime=0;
	SDA_IN();      //ÅäÖÃSDAÏßÎªÊäÈë  
	IIC_SDAOUT=1;
	delay_us(1);	   
	IIC_SCL=1;
	delay_us(1);	 
	while(IIC_SDAIN)
	{
		WaitTime++;
		if(WaitTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0; 
	return 0;  
}
/****************************************************************************
* Ãû    ³Æ: u8 void MCU_Send_Ack(void)
* ¹¦    ÄÜ£ºMCU²úÉúACKÓ¦´ð,¸æÖª24cxx
* Èë¿Ú²ÎÊý£ºÎÞ
* ·µ»Ø²ÎÊý£º
* Ëµ    Ã÷£º  £B
****************************************************************************/
void MCU_Send_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDAOUT=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
/****************************************************************************
* Ãû    ³Æ: u8 void MCU_Send_Ack(void)
* ¹¦    ÄÜ£ºMCU²»²úÉúACKÓ¦´ð	
* Èë¿Ú²ÎÊý£ºÎÞ
* ·µ»Ø²ÎÊý£º
* Ëµ    Ã÷£º  £B
****************************************************************************/  
void MCU_NOAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDAOUT=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}	
/****************************************************************************
* Ãû    ³Æ: void IIC_write_OneByte(u8 Senddata)
* ¹¦    ÄÜ£ºIICÐ´Ò»¸ö×Ö½Úµ½EEPROM	
* Èë¿Ú²ÎÊý£ºSenddata:Ð´ÈëµÄ8Î»Êý¾Ý
* ·µ»Ø²ÎÊý£º
* Ëµ    Ã÷£º  £B
****************************************************************************/	  
void IIC_write_OneByte(u8 Senddata)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;    //À­µÍÊ±ÖÓ¿ªÊ¼Êý¾Ý´«Êä
    for(t=0;t<8;t++)
    {              
        IIC_SDAOUT=(Senddata&0x80)>>7;
        Senddata<<=1; 	  
		delay_us(2);   
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 
/****************************************************************************
* Ãû    ³Æ: void IIC_Read_OneByte(u8 Senddata)
* ¹¦    ÄÜ£ºIIC¶ÁÈ¡Ò»¸ö×Ö½Ú
* Èë¿Ú²ÎÊý£ºack=1£¬·¢ËÍACK£¬ack=0£¬·¢ËÍnACK 
* ·µ»Ø²ÎÊý£º¶Áµ½µÄ8Î»Êý¾Ý
* Ëµ    Ã÷£º  £B
****************************************************************************/	  
u8 IIC_Read_OneByte(u8 ack)
{
	u8 i,receivedata=0;
	SDA_IN();       //ÅäÖÃSDAÏßÎªÊäÈë  
    for(i=0;i<8;i++ )
	  {
        IIC_SCL=0; 
        delay_us(2);
		    IIC_SCL=1;
        receivedata<<=1;
        if(IIC_SDAIN)receivedata++;   
		delay_us(1); 
    }					 
    if (!ack)
        MCU_NOAck();//·¢ËÍnACK
    else
        MCU_Send_Ack(); //·¢ËÍACK   
    return receivedata;
}
/*******************************IO¿ÚÄ£ÄâIIC*************************************
*******************************************************************************/


/*******************************************************************************
*************************ÒÔÏÂÎªEEPROM24C02¶ÁÐ´²Ù×÷******************************
*******************************************************************************/
//³õÊ¼»¯24c02µÄIIC½Ó¿Ú
void AT24C02_Init(void)
{
	IIC_Init();  //IIC³õÊ¼»¯
}
/****************************************************************************
* Ãû    ³Æ: u8 AT24C02_ReadByte(u8 ReadAddr)
* ¹¦    ÄÜ£ºÔÚAT24C02Ö¸¶¨µØÖ·¶Á³öÒ»¸öÊý¾Ý
* Èë¿Ú²ÎÊý£ºReadAddr£ºÒª¶ÁÈ¡Êý¾ÝËùÔÚµÄµØÖ·
* ·µ»Ø²ÎÊý£º¶Áµ½µÄ8Î»Êý¾Ý
* Ëµ    Ã÷£º  £B
****************************************************************************/
u8 AT24C02_ReadByte(u16 ReadAddr)
{				  
	u8 receivedata=0;		  	    																 
  
	IIC_Start();  
	IIC_write_OneByte(0XA0);           //·¢ËÍÆ÷¼þµØÖ·0XA0
	MCU_Wait_Ack();
  IIC_write_OneByte((uint8_t)(ReadAddr & 0x00FF));       //·¢ËÍµÍµØÖ·
	MCU_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_write_OneByte(0XA1);           //½øÈë½ÓÊÕÄ£Ê½			   
	MCU_Wait_Ack();	 
  receivedata=IIC_Read_OneByte(0);		   
  IIC_Stop();                    //²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ	    
	
	return receivedata;
}
/****************************************************************************
* Ãû    ³Æ: u8 AT24C02_WriteByte(u8 WriteAddr,u8 WriteData)
* ¹¦    ÄÜ£ºÔÚAT24C02Ö¸¶¨µØÖ·Ð´ÈëÒ»¸öÊý¾Ý
* Èë¿Ú²ÎÊý£ºWriteAddr£ºÒªÐ´ÈëÊý¾ÝËùÔÚµÄµØÖ·
            WriteData: ÒªÐ´ÈëµÄÊý¾Ý
* ·µ»Ø²ÎÊý£º 
* Ëµ    Ã÷£º  £B
****************************************************************************/
void AT24C02_WriteByte(u16 WriteAddr,u8 WriteData)
{				   	  	    																 
  IIC_Start();  
	IIC_write_OneByte(0XA0);       //·¢ËÍ0XA0,Ð´Êý¾Ý 	 
	MCU_Wait_Ack();	   
  IIC_write_OneByte((uint8_t)(WriteAddr & 0x00FF));  //·¢ËÍµÍµØÖ·
	MCU_Wait_Ack(); 	 										  		   
	IIC_write_OneByte(WriteData);  //·¢ËÍ×Ö½Ú							   
	MCU_Wait_Ack();  		    	   
  IIC_Stop();                    //²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ 
	delay_ms(10);	 
}
/****************************************************************************
* Ãû    ³Æ: u8 AT24C02_Test(void)
* ¹¦    ÄÜ£º²âÊÔAT24C02ÊÇ·ñÕý³£
* Èë¿Ú²ÎÊý£ºÎÞ
* ·µ»Ø²ÎÊý£º·µ»Ø1:¼ì²âÊ§°Ü
            ·µ»Ø0:¼ì²â³É¹¦ 
* Ëµ    Ã÷£º  £B
****************************************************************************/
u8 AT24C02_Test(void)
{
	u8 Testdata;
	Testdata=AT24C02_ReadByte(255); //Èç¹û¿ª»ú²âÊÔ£¬ÒÑÓÐÖµÎÞÐèÔÙ´ÎÐ´Èë	   
	if(Testdata==0XAB)return 0;		   
	else                             
	{
		AT24C02_WriteByte(255,0XAB);
	  Testdata=AT24C02_ReadByte(255);	  
		if(Testdata==0XAB)return 0;
	}
	return 1;											  
}
/****************************************************************************
* Ãû    ³Æ: u32 Buf_4Byte(u8 *pBuffer,u32 Date_4Byte,u8 Byte_num,u8 mode)
* ¹¦    ÄÜ£º¶àÎ»ÊýÓë×Ö½Ú»¥×ª
* Èë¿Ú²ÎÊý£ºmode£º1:¶àÎ»Êý×ª·Ö³É×Ö½Ú   0:×Ö½ÚºÏ²¢³ÉÒ»¸ö¶àÎ»Êý
            Byte_num£ºÐèÒª×ª»¯µÄ×Ö½ÚÊý
            *pBuffer£º×Ö½Ú½ÓÊÕÊý×é»ò×Ö½ÚËùÔÚÊý×é
            Date_4Byte£º¶àÎ»ÊýÊý
* ·µ»Ø²ÎÊý£ºmodeÎª0Ê±£¬·µ»Ø¶àÎ»Êý
* Ëµ    Ã÷£ºByte_num×î´óÎª4¸ö×Ö½Ú£¬¸Ãº¯ÊýÔÚºóÃæµÄ´¥ÃþÆÁÐ£ÕýÊ±´æÈ¡Ð£ÕýÖµËùÓÃµ½
****************************************************************************/
u32 Buf_4Byte(u8 *pBuffer,u32 Date_4Byte,u8 Byte_num,u8 mode)
{
   u8 i; u32 middata=0;
	if(mode)    //¶àÎ»Êý×ª·Ö³É×Ö½Ú
	 {
	   for(i=0;i<Byte_num;i++)
	     {
	       *pBuffer++ =(Date_4Byte>>(8*i))&0xff;
	     }
			return 0; 
	 } 
	 else       //×Ö½ÚºÏ²¢³ÉÒ»¸ö¶àÎ»Êý
	 {
	    Date_4Byte=0;
		  pBuffer+=(Byte_num-1);
		  for(i=0;i<Byte_num;i++)
	      { 		
		      middata<<=8;
		      middata+= *pBuffer--;			   
	      }
			return middata;	
	 }
}
/****************************************************************************
* Ãû    ³Æ: void AT24C02_Read(u8 ReadAddr,u8 *pBuffer,u8 ReadNum)
* ¹¦    ÄÜ£º´ÓAT24C02ÀïÃæµÄÖ¸¶¨µØÖ·¿ªÊ¼¶Á³öÖ¸¶¨¸öÊýµÄÊý¾Ý
* Èë¿Ú²ÎÊý£ºReadAddr :¿ªÊ¼¶Á³öµÄµØÖ·  0~255
            pBuffer  :Êý¾ÝÊý×éÊ×µØÖ·
            ReadNum:Òª¶Á³öÊý¾ÝµÄ¸öÊý
* ·µ»Ø²ÎÊý£º
* Ëµ    Ã÷£º  £B
****************************************************************************/
void AT24C02_Read(u16 ReadAddr,u8 *pBuffer,u16 ReadNum)
{
	while(ReadNum--)
	{
		*pBuffer++=AT24C02_ReadByte(ReadAddr++);	
	}
} 
/****************************************************************************
* Ãû    ³Æ: void AT24C02_Write(u8 WriteAddr,u8 *pBuffer,u8 WriteNum)
* ¹¦    ÄÜ£º´ÓAT24C02ÀïÃæµÄÖ¸¶¨µØÖ·¿ªÊ¼Ð´ÈëÖ¸¶¨¸öÊýµÄÊý¾Ý
* Èë¿Ú²ÎÊý£ºWriteAddr :¿ªÊ¼Ð´ÈëµÄµØÖ·  0~255
            pBuffer  :Êý¾ÝÊý×éÊ×µØÖ·
            WriteNum:ÒªÐ´ÈëÊý¾ÝµÄ¸öÊý
* ·µ»Ø²ÎÊý£º
* Ëµ    Ã÷£º  £B
****************************************************************************/
void AT24C02_Write(u16 WriteAddr,u8 *pBuffer,u16 WriteNum)
{
	while(WriteNum--)
	{
		AT24C02_WriteByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








