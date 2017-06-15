#include "EIIC.h"
//#include "delay.h"

//初始化IIC
void EIIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

  //GPIOB8,B9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	EIIC_SCL=1;
	EIIC_SDA=1;
}
//产生IIC起始信号
void EIIC_Start(void)
{
	ESDA_OUT();     //sda线输出
	EIIC_SDA=1;	  	  
	EIIC_SCL=1;
	delay_us(4);
 	EIIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	EIIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void EIIC_Stop(void)
{
	ESDA_OUT();//sda线输出
	EIIC_SCL=0;
	EIIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	EIIC_SCL=1; 
	EIIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 EIIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	ESDA_IN();      //SDA设置为输入  
	EIIC_SDA=1;delay_us(1);	   
	EIIC_SCL=1;delay_us(1);	 
	while(EREAD_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			EIIC_Stop();
			return 1;
		}
	}
	EIIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void EIIC_Ack(void)
{
	EIIC_SCL=0;
	ESDA_OUT();
	EIIC_SDA=0;
	delay_us(2);
	EIIC_SCL=1;
	delay_us(2);
	EIIC_SCL=0;
}
//不产生ACK应答		    
void EIIC_NAck(void)
{
	EIIC_SCL=0;
	ESDA_OUT();
	EIIC_SDA=1;
	delay_us(2);
	EIIC_SCL=1;
	delay_us(2);
	EIIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void EIIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	ESDA_OUT(); 	    
    EIIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        EIIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
			
		delay_us(2);   //对TEA5767这三个延时都是必须的
		EIIC_SCL=1;
		delay_us(2); 
		EIIC_SCL=0;	
		delay_us(2);
		delay_ms(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 EIIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	ESDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        EIIC_SCL=0; 
        delay_us(2);
		EIIC_SCL=1;
        receive<<=1;
        if(EREAD_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        EIIC_NAck();//发送nACK
    else
        EIIC_Ack(); //发送ACK   
    return receive;
}
