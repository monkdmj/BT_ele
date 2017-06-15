#include "eeprom.h"
//#include "delay.h"


//初始化IIC接口
void AT24CXX_Init(void)
{
	EIIC_Init();//IIC初始化
}


u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{                 
    u8 temp=0;                                                                               
    EIIC_Start();  
    if(EE_TYPE>AT24C16)
    {
        EIIC_Send_Byte(0XA0);       
        EIIC_Wait_Ack();
        EIIC_Send_Byte(ReadAddr>>8);      
    }else EIIC_Send_Byte(0XA0+((ReadAddr/256)<<1));        
    EIIC_Wait_Ack(); 
    EIIC_Send_Byte(ReadAddr%256);   
    EIIC_Wait_Ack();     
    EIIC_Start();           
    if(EE_TYPE>AT24C16)EIIC_Send_Byte(0XA1);        
    else EIIC_Send_Byte(0XA1+((ReadAddr/256)<<1));   
    EIIC_Wait_Ack();  
    temp=EIIC_Read_Byte(0);         
    EIIC_Stop();    
    return temp;
}

void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{                                                                                            
    EIIC_Start();  
    if(EE_TYPE>AT24C16)
    {
        EIIC_Send_Byte(0XA0);        
        EIIC_Wait_Ack();
        EIIC_Send_Byte(WriteAddr>>8);    
    }else EIIC_Send_Byte(0XA0+((WriteAddr/256)<<1));       
    EIIC_Wait_Ack();    
    EIIC_Send_Byte(WriteAddr%256);   
    EIIC_Wait_Ack();                                                        
    EIIC_Send_Byte(DataToWrite);                              
    EIIC_Wait_Ack();                    
    EIIC_Stop();
    delay_ms(10);    
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
		delay_ms(100);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c08为0~1024
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

u8 EEPROM_Test(void)
{
		u8 pbuffer[10] = {0,1,2,3,4,5,6,7,8,9};
		u8 temp[10];
		AT24CXX_Write(0,pbuffer,10);
		delay_ms(100);
		AT24CXX_Read(0,temp,10);
		for(u8 i = 0; i < 10 ; i++)
		{
				if(temp[i] != pbuffer[i])
					return 0;
		}
		return 1;
}