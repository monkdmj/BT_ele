#include "spi.h"
#include "w25qxx.h"  

/*********************************************************************************
*************************MCUÆôÃ÷ STM32F407ºËÐÄ¿ª·¢°å******************************
**********************************************************************************
* ÎÄ¼þÃû³Æ: w25qxx.c                                                             *
* ÎÄ¼þ¼òÊö£ºW25Q128Ê¹ÓÃ                                                          *
* ´´½¨ÈÕÆÚ£º2015.03.10                                                           *
* °æ    ±¾£ºV1.0                                                                 *
* ×÷    Õß£ºClever                                                               *
* Ëµ    Ã÷£º25Q128ÈÝÁ¿Îª16M,¹²ÓÐ128¸öBlock,4096¸öSector                          *
            255byteÎªÒ»Ò³                                                        *
						4KbytesÎªÒ»¸öÉÈÇø                                                    *
            16¸öÉÈÇøÎªÒ»¸ö¿é                                                     *
*********************************************************************************/	
 
u16 W25QXX_ID;	 
													 
//³õÊ¼»¯SPI FLASHµÄIO¿Ú
void W25QXX_Init(void)
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //Ê¹ÄÜGPIOGÊ±ÖÓ

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;             //PG8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         //Êä³ö
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //ÍÆÍìÊä³ö
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //ÉÏÀ­
  GPIO_Init(GPIOC, &GPIO_InitStructure);                //³õÊ¼»¯

	W25QXX_CS=1;			                                    //SPI FLASH²»Ñ¡ÖÐ
	SPI1_Init();		   			                              //³õÊ¼»¯SPI
	SPI1_Setclock(SPI_BaudRatePrescaler_4);		            //ÉèÖÃÎª21MÊ±ÖÓ,¸ßËÙÄ£Ê½ 
	W25QXX_ID=W25QXX_ReadID();	                          //¶ÁÈ¡FLASH ID.
}  
/****************************************************************************
* Ãû    ³Æ: u8 W25QXX_ReadSR(void) 
* ¹¦    ÄÜ£º¶ÁÈ¡W25QXXµÄ×´Ì¬¼Ä´æÆ÷
* Èë¿Ú²ÎÊý£ºÎÞ
* ·µ»Ø²ÎÊý£º×´Ì¬¼Ä´æÆ÷µÄÖµ
* Ëµ    Ã÷£º 		     
****************************************************************************/
u8 W25QXX_ReadSR(void)   
{  
	u8 byte=0;   
	W25QXX_CS=0;                                  //Ê¹ÄÜÆ÷¼þ   
	SPI1_ReadWriteByte(W25X_CMD_ReadStatusReg);   //·¢ËÍ¶ÁÈ¡×´Ì¬¼Ä´æÆ÷ÃüÁî    
	byte=SPI1_ReadWriteByte(0Xff);                //¶ÁÈ¡Ò»¸ö×Ö½Ú  
	W25QXX_CS=1;                                  //È¡ÏûÆ¬Ñ¡     
	return byte;   
} 
/****************************************************************************
* Ãû    ³Æ: void W25QXX_Write_SR(u8 sr)
* ¹¦    ÄÜ£ºÐ´W25QXX×´Ì¬¼Ä´æÆ÷
* Èë¿Ú²ÎÊý£ºÐ´ÈëµÄÖµ
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 		     
****************************************************************************/
void W25QXX_Write_SR(u8 sr)   
{   
	W25QXX_CS=0;                                 //Ê¹ÄÜÆ÷¼þ   
	SPI1_ReadWriteByte(W25X_CMD_WriteStatusReg); //·¢ËÍÐ´È¡×´Ì¬¼Ä´æÆ÷ÃüÁî    
	SPI1_ReadWriteByte(sr);                      //Ð´ÈëÒ»¸ö×Ö½Ú  
	W25QXX_CS=1;                                 //È¡ÏûÆ¬Ñ¡     	      
} 

//W25QXXÐ´Ê¹ÄÜ	 
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                               //Ê¹ÄÜÆ÷¼þ   
  SPI1_ReadWriteByte(W25X_CMD_WriteEnable);  //·¢ËÍÐ´Ê¹ÄÜ  
	W25QXX_CS=1;                               //È¡ÏûÆ¬Ñ¡     	      
} 
//µÈ´ý¿ÕÏÐ
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);      //µÈ´ýBUSYÎ»Çå¿Õ
} 
//W25QXXÐ´½ûÖ¹	 
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                                //Ê¹ÄÜÆ÷¼þ   
  SPI1_ReadWriteByte(W25X_CMD_WriteDisable);  //·¢ËÍÐ´½ûÖ¹Ö¸Áî    
	W25QXX_CS=1;                                //È¡ÏûÆ¬Ñ¡     	      
} 
/****************************************************************************
* Ãû    ³Æ: u16 W25QXX_ReadID(void)
* ¹¦    ÄÜ£º¶ÁÈ¡Ð¾Æ¬ID
* Èë¿Ú²ÎÊý£ºÎÞ
* ·µ»Ø²ÎÊý£ºÐ¾Æ¬ID
* Ëµ    Ã÷£º0XEF13,±íÊ¾Ð¾Æ¬ÐÍºÅÎªW25Q80  
            0XEF14,±íÊ¾Ð¾Æ¬ÐÍºÅÎªW25Q16    
            0XEF15,±íÊ¾Ð¾Æ¬ÐÍºÅÎªW25Q32  
            0XEF16,±íÊ¾Ð¾Æ¬ÐÍºÅÎªW25Q64 
            0XEF17,±íÊ¾Ð¾Æ¬ÐÍºÅÎªW25Q128      
****************************************************************************/  
u16 W25QXX_ReadID(void)
{
	u16 IDnum = 0;	  
	W25QXX_CS=0;				    
	SPI1_ReadWriteByte(0x90); //·¢ËÍ¶ÁÈ¡IDÃüÁî	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	IDnum|=SPI1_ReadWriteByte(0xFF)<<8;  
	IDnum|=SPI1_ReadWriteByte(0xFF);	 
	W25QXX_CS=1;				    
	return IDnum;
} 
/****************************************************************************
* Ãû    ³Æ: void W25QXX_Erase_Chip(void) 
* ¹¦    ÄÜ£º²Á³ýÕû¸öÐ¾Æ¬		  
* Èë¿Ú²ÎÊý£ºÎÞ
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 		     
****************************************************************************/
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                   
    W25QXX_Wait_Busy();   
  	W25QXX_CS=0;                             //Ê¹ÄÜÆ÷¼þ   
    SPI1_ReadWriteByte(W25X_CMD_ChipErase);  //·¢ËÍÆ¬²Á³ýÃüÁî  
	  W25QXX_CS=1;                             //È¡ÏûÆ¬Ñ¡     	      
	  W25QXX_Wait_Busy();   				           //µÈ´ýÐ¾Æ¬²Á³ý½áÊø
}  
/****************************************************************************
* Ãû    ³Æ: void W25QXX_Erase_Sector(u32 First_Addr)
* ¹¦    ÄÜ£º²Á³ýÄ³¸öÉÈÇøµÄÊ×µØÖ·	  
* Èë¿Ú²ÎÊý£ºFirst_Addr:ÉÈÇøµØÖ·
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 		     
****************************************************************************/
void W25QXX_Erase_Sector(u32 First_Addr)   
{    
 	  First_Addr*=4096;
    W25QXX_Write_Enable();                  
    W25QXX_Wait_Busy();   
  	W25QXX_CS=0;                                  //Ê¹ÄÜÆ÷¼þ   
    SPI1_ReadWriteByte(W25X_CMD_SectorErase);     //·¢ËÍÉÈÇø²Á³ýÖ¸Áî 
    SPI1_ReadWriteByte((u8)((First_Addr)>>16));   //·¢ËÍµØÖ·    
    SPI1_ReadWriteByte((u8)((First_Addr)>>8));   
    SPI1_ReadWriteByte((u8)First_Addr);  
	  W25QXX_CS=1;                                  //È¡ÏûÆ¬Ñ¡     	      
    W25QXX_Wait_Busy();   				                //µÈ´ý²Á³ýÍê³É
}  
//½øÈëµôµçÄ£Ê½
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS=0;                             //Ê¹ÄÜÆ÷¼þ   
    SPI1_ReadWriteByte(W25X_CMD_PowerDown);  //·¢ËÍµôµçÃüÁî  
	  W25QXX_CS=1;                             //È¡ÏûÆ¬Ñ¡     	      
    delay_us(3);                             
}   
//»½ÐÑ
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS=0;                                      //Ê¹ÄÜÆ÷¼þ   
    SPI1_ReadWriteByte(W25X_CMD_ReleasePowerDown);    //·¢ËÍ»½ÐÑÃüÁî
	  W25QXX_CS=1;                                      //È¡ÏûÆ¬Ñ¡     	      
    delay_us(3);                                     
}   

/****************************************************************************
* Ãû    ³Æ: void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
* ¹¦    ÄÜ£ºÖ¸¶¨µØÖ·¿ªÊ¼¶ÁÈ¡Ö¸¶¨³¤¶ÈµÄÊý¾Ý
* Èë¿Ú²ÎÊý£ºDataBuffer:Êý¾Ý´æ´¢Çø
            StartAddress:¿ªÊ¼¶ÁÈ¡µÄµØÖ·(24bit)
            ByteCount:Òª¶ÁÈ¡µÄ×Ö½ÚÊý(×î´ó65535)
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 		     
****************************************************************************/
void W25QXX_Read(u8* DataBuffer,u32 StartAddress,u16 ByteCount)   
{  										    
	W25QXX_CS=0;                                 //Ê¹ÄÜÆ÷¼þ   
    SPI1_ReadWriteByte(W25X_CMD_ReadData);     //·¢ËÍ¶ÁÈ¡ÃüÁî   
    SPI1_ReadWriteByte((u8)((StartAddress)>>16));  //·¢ËÍ24bitµØÖ·    
    SPI1_ReadWriteByte((u8)((StartAddress)>>8));   
    SPI1_ReadWriteByte((u8)StartAddress);   
	
			while (ByteCount--) 
			{
					*DataBuffer = SPI1_ReadWriteByte(0XFF);
					DataBuffer++;
			}		
	W25QXX_CS=1;  				    	      
}  
/****************************************************************************
* Ãû    ³Æ: void W25QXX_Write_Page(u8* DataBuffer,u32 StartAddress,u16 ByteCount)
* ¹¦    ÄÜ£ºÔÚÒ»Ò³ÄÚÐ´ÈëÉÙÓÚ256¸ö×Ö½ÚµÄÊý¾Ý  
* Èë¿Ú²ÎÊý£ºDataBuffer:Êý¾Ý´æ´¢Çø
            StartAddress:¿ªÊ¼Ð´ÈëµÄµØÖ·(24bit)
            ByteCount:ÒªÐ´ÈëµÄ×Ö½ÚÊý(×î´ó256)
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º 		     
****************************************************************************/
void W25QXX_Write_Page(u8* DataBuffer,u32 StartAddress,u16 ByteCount)
{  
	W25QXX_Write_Enable();                   
	  W25QXX_CS=0;                                   //Ê¹ÄÜÆ÷¼þ   
    SPI1_ReadWriteByte(W25X_CMD_PageProgram);      //·¢ËÍÐ´Ò³ÃüÁî   
    SPI1_ReadWriteByte((u8)((StartAddress)>>16));  //·¢ËÍµØÖ·    
    SPI1_ReadWriteByte((u8)((StartAddress)>>8));   
    SPI1_ReadWriteByte((u8)StartAddress);   
		while (ByteCount--)
			{
			  SPI1_ReadWriteByte(*DataBuffer);
			  DataBuffer++;
			}	
	W25QXX_CS=1;                   //È¡ÏûÆ¬Ñ¡ 
	W25QXX_Wait_Busy();					   //µÈ´ýÐ´Èë½áÊø
}
/****************************************************************************
* Ãû    ³Æ: void W25QXX_PageWrite(u8* DataBuffer,u32 StartAddress,u16 ByteCount) 
* ¹¦    ÄÜ£º°´Ò³Ð´ÈëÊý¾Ý
* Èë¿Ú²ÎÊý£ºDataBuffer:Êý¾Ý´æ´¢Çø
            StartAddress:¿ªÊ¼Ð´ÈëµÄµØÖ·(24bit)
            ByteCount:ÒªÐ´ÈëµÄ×Ö½ÚÊý(×î´ó256)
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£ºÓÐ×Ô¶¯»»Ò³¹¦ÄÜ£¬ÒòÎª¸Ãº¯ÊýÃ»ÓÐÔÚÐ´Ö®Ç°ÅÐ¶ÏÒªÐ´ÈëµÄµØÖ·ÉÏµÄÊý¾ÝÊÇ·ñÎª
            ¿Õ£¬ËùÒÔÔÚÐ´ÈëÖ®Ç°±ØÐëÈ·±£¸ÃµØÖ·ÉÏµÄÊý¾ÝÎª¿Õ0xFF£¬·ñÔòÐ´ÈëÊ§°Ü 		     
****************************************************************************/
void W25QXX_PageWrite(u8* DataBuffer,u32 StartAddress,u16 ByteCount)   
{
  u8 NumOfPage = 0, NumOfSingle = 0, Surplus_Addr = 0, Surplus_count = 0, midtemp = 0;

  Surplus_Addr = StartAddress % 256;
  Surplus_count = 256 - Surplus_Addr;
  NumOfPage =  ByteCount / 256;
  NumOfSingle = ByteCount % 256;

  if (Surplus_Addr == 0) //¿ªÊ¼Ð´µÄµØÖ·¸ÕºÃÊÇÒ³¿ªÊ¼µÄµØÖ·
  {
    if (NumOfPage == 0)  // ByteCount < 256£¨Ò»Ò³×ÜµÄ×Ö½ÚÊý£©  
    {
      W25QXX_Write_Page(DataBuffer, StartAddress, ByteCount);
    }
    else                 // ByteCount > 256£¨Ò»Ò³×ÜµÄ×Ö½ÚÊý£©  
    {
      while (NumOfPage--)
      {
        W25QXX_Write_Page(DataBuffer, StartAddress, 256);
        StartAddress +=  256;
        DataBuffer += 256;
      }
      W25QXX_Write_Page(DataBuffer, StartAddress, NumOfSingle);
    }
  }
  else ///¿ªÊ¼Ð´µÄµØÖ·²»ÔÚÒ³µÄÊ×µØÖ·ÉÏ
  {
    if (NumOfPage == 0) // ByteCount < 256£¨Ò»Ò³×ÜµÄ×Ö½ÚÊý£© 
    {
      if (NumOfSingle > Surplus_count)  
      {
        midtemp = NumOfSingle - Surplus_count;
        W25QXX_Write_Page(DataBuffer, StartAddress, Surplus_count);
        StartAddress +=  Surplus_count;
        DataBuffer += Surplus_count;
        W25QXX_Write_Page(DataBuffer, StartAddress, midtemp);
      }
      else
      {
        W25QXX_Write_Page(DataBuffer, StartAddress, ByteCount);
      }
    }
    else //ByteCount > 256£¨Ò»Ò³×ÜµÄ×Ö½ÚÊý£©  
    {
      ByteCount -= Surplus_count;
      NumOfPage =  ByteCount / 256;
      NumOfSingle = ByteCount % 256;

      W25QXX_Write_Page(DataBuffer, StartAddress, Surplus_count);
      StartAddress +=  Surplus_count;
      DataBuffer += Surplus_count;
      while (NumOfPage--)
      {
        W25QXX_Write_Page(DataBuffer, StartAddress, 256);
        StartAddress +=  256;
        DataBuffer += 256;
      }
      if (NumOfSingle != 0)
      {
        W25QXX_Write_Page(DataBuffer, StartAddress, NumOfSingle);
      }
    }
  }
} 
/****************************************************************************
* Ãû    ³Æ: void W25QXX_SectorWrite(u8* DataBuffer,u32 StartAddress,u16 ByteCount)
* ¹¦    ÄÜ£º°´ÉÈÇøÐ´ÈëÊý¾Ý
* Èë¿Ú²ÎÊý£ºDataBuffer:Êý¾Ý´æ´¢Çø
            StartAddress:¿ªÊ¼Ð´ÈëµÄµØÖ·(24bit)
            ByteCount:ÒªÐ´ÈëµÄ×Ö½ÚÊý(×î´ó65536)
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£ºÐ´ÈëÖ®Ç°ÅÐ¶Ï¸ÄµØÖ·ÉÏµÄÊý¾ÝÊÇ·ñÎª¿Õ£¬²»ÊÇÏÈ²Á³ýÔÙÐ´Èë£¬¸Ãº¯Êý²Î¿¼ÍøÉÏ´úÂëhttp://www.openedv.com      
****************************************************************************/
u8 TS_BUFFER[4096];	
void W25QXX_SectorWrite(u8* DataBuffer,u32 StartAddress,u16 ByteCount)   
{ 
	u32 secaddr;
	u16 secused;
	u16 Surplus_count;	   
 	u16 i=0; 
	
	u8 * Cache_BUF;	   
  Cache_BUF=TS_BUFFER;	  
	
 	secaddr=StartAddress/4096;//ÉÈÇøµØÖ·  
	secused=StartAddress%4096;//Ð´ÈëµÄµØÖ·ÔÚÉÈÇøÄÚµÄÆ«ÒÆ£¨´Ó¸ÃÉÈÇøµÄÊ×µØÖ·¿ªÊ¼ÒÑÊ¹ÓÃµÄ×Ö½ÚÊý£©
	Surplus_count=4096-secused;//¸ÃÉÈÇøÊ£Óà¿Õ¼ä´óÐ¡   

 	if(ByteCount<=Surplus_count)Surplus_count=ByteCount;//²»´óÓÚ4096¸ö×Ö½Ú
	while(1) 
	{	
		W25QXX_Read(Cache_BUF,secaddr*4096,4096);//¶Á³öÕû¸öÉÈÇøµÄÄÚÈÝ
		      i=0;
		while(Cache_BUF[secused+i]==0XFF)
				{
					i++;
					if(i==Surplus_count)break;
				}
		if(i<Surplus_count)//ÐèÒª²Á³ý
				{
					W25QXX_Erase_Sector(secaddr);//²Á³ýÕâ¸öÉÈÇø
					for(i=0;i<Surplus_count;i++)	   //¸´ÖÆ
					{
						Cache_BUF[i+secused]=DataBuffer[i];	  
					}
					W25QXX_PageWrite(Cache_BUF,secaddr*4096,4096);//Ð´ÈëÕû¸öÉÈÇø  

				}
		else
			  W25QXX_PageWrite(DataBuffer,StartAddress,Surplus_count);//Ð´ÒÑ¾­²Á³ýÁËµÄ,Ö±½ÓÐ´ÈëÉÈÇøÊ£ÓàÇø¼ä. 				   
		if(ByteCount==Surplus_count)break;//Ð´Èë½áÊøÁË
		else//Ð´ÈëÎ´½áÊø
				{
					secaddr++;//ÉÈÇøµØÖ·Ôö1
					secused=0;//Æ«ÒÆÎ»ÖÃÎª0 	 

					DataBuffer+=Surplus_count;  //Ö¸ÕëÆ«ÒÆ
					StartAddress+=Surplus_count;//Ð´µØÖ·Æ«ÒÆ	   
					ByteCount-=Surplus_count;				//×Ö½ÚÊýµÝ¼õ
					if(ByteCount>4096)Surplus_count=4096;	//ÏÂÒ»¸öÉÈÇø»¹ÊÇÐ´²»Íê
					else Surplus_count=ByteCount;			//ÏÂÒ»¸öÉÈÇø¿ÉÒÔÐ´ÍêÁË
				}	 
	};
}

