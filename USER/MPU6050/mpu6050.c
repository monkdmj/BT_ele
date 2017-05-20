#include "mpu6050.h"
#include "led.h"

float a[3] = {0.0f};
float w[3] = {0.0f};
float angle[3] = {0.0f};
float T = 0.0f;
u8 temp_buf[33],result_buf[33];
u32 sign = 0;
u8 Temp[33];
u32 u6_count = 0;
/*
u8 hes;
u8 sumone = 0;
u8 sumtwo = 0;
u8 sumthree = 0;
*/

//串口2初始化 PC6-USART6_TX  PC7-USART6_RX
void USART6_Config(void)   //初始化 配置USART2
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOB时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE); //使能USART6时钟 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);  //GPIOC6复用为USART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC7复用为USART6
	//USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOC6与GPIOC7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);          //初始化PC6，PC7
   //USART6 初始化设置
	USART_InitStructure.USART_BaudRate = 9600;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART6, &USART_InitStructure); //初始化串口6	
    USART_Cmd(USART6, ENABLE);  //使能串口6 
	
	USART_ClearFlag(USART6, USART_FLAG_TC);
	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);         //开启相关中断
	//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;      //串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		   //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	  //根据指定的参数初始化VIC寄存器、
}

void USART6_IRQHandler(void)
{
	//LED2=!LED2;
	if (USART_GetITStatus(USART6,USART_IT_RXNE) != RESET)
	{
		Temp[u6_count] = USART_ReceiveData(USART6);
		if (Temp[u6_count] != 0x55 && u6_count == 0)
		{
			return;
		}
		u6_count ++;
		if (u6_count == 11)
		{
			sign = 1;
			u6_count = 0;
			memcpy(temp_buf,Temp,11);
		}
		/*
		if (u6_count == 33)
		{
			u32 i;
	        for (i = 0 ; i < 10 ; i ++)
			{
				sumone += Temp[i];
				sumtwo += Temp[i + 11];
				sumthree += Temp[i + 22];
			}
			if (sumone == Temp[10] && sumtwo == Temp[21] && sumthree == Temp[32])
			{
				sign = 1;
			    u6_count = 0;
			    memcpy(temp_buf,Temp,33);
				sumone = 0;
				sumtwo = 0;
				sumthree = 0;
			}
			else 
			{
				sign = 0;
				u6_count = 0;
				sumone = 0;
				sumtwo = 0;
				sumthree = 0;
			} 
		}
		*/
	}
	if (sign == 1)
	{
		sign = 0;
		memcpy(result_buf,temp_buf,11);
		if (result_buf[0] == 0x55)
		{
			switch(result_buf[1])
			{
				case 0x51:
					a[0] = ((short)(result_buf[3]<<8 | result_buf[2]))/32768.0*16;      //X轴加速度
		            a[1] = ((short)(result_buf[5]<<8 | result_buf[4]))/32768.0*16;      //Y轴加速度
		            a[2] = ((short)(result_buf[7]<<8 | result_buf[6]))/32768.0*16;      //Z轴加速度
		            T    = ((short)(result_buf[9]<<8 | result_buf[8]))/340.0+36.25;      //温度
					break;
				case 0x52:
					w[0] = ((short)(result_buf[3]<<8 | result_buf[2]))/32768.0*2000;      //X轴角速度
	                w[1] = ((short)(result_buf[5]<<8 | result_buf[4]))/32768.0*2000;      //Y轴角速度
	                w[2] = ((short)(result_buf[7]<<8 | result_buf[6]))/32768.0*2000;      //Z轴角速度
	                T    = ((short)(result_buf[9]<<8 | result_buf[8]))/340.0+36.25;      //温度
					break;
				case 0x53:
					angle[0] = ((short)(result_buf[3]<<8 | result_buf[2]))/32768.0*180;   //X轴滚转角（x 轴）
                    angle[1] = ((short)(result_buf[5]<<8 | result_buf[4]))/32768.0*180;   //Y轴俯仰角（y 轴）
                    angle[2] = ((short)(result_buf[7]<<8 | result_buf[6]))/32768.0*180;   //Z轴偏航角（z 轴）
                    T    = ((short)(result_buf[9]<<8 | result_buf[8]))/340.0+36.25;      //温度
					break;	        
				default: 
					break;
			}
		}
	}
	/*
	if (sign == 1)
	{
		sign = 0;
		memcpy(result_buf,temp_buf,33);
		if (result_buf[0] == 0x55)
		{
			if (result_buf[1] == 0x51)
			{
				a[0] = ((short)(result_buf[3]<<8 | result_buf[2]))/32768.0*16;      //X轴加速度
	            a[1] = ((short)(result_buf[5]<<8 | result_buf[4]))/32768.0*16;      //Y轴加速度
	            a[2] = ((short)(result_buf[7]<<8 | result_buf[6]))/32768.0*16;      //Z轴加速度
				
				if (result_buf[11] == 0x55)
				{
					if (result_buf[12] == 0x52)
					{
						w[0] = ((short)(result_buf[14]<<8 | result_buf[13]))/32768.0*2000;      //X轴角速度
		                w[1] = ((short)(result_buf[16]<<8 | result_buf[15]))/32768.0*2000;      //Y轴角速度
		                w[2] = ((short)(result_buf[18]<<8 | result_buf[17]))/32768.0*2000;      //Z轴角速度
					    
						if (result_buf[22] == 0x55)
						{
							if (result_buf[23] == 0x53)
							{
								angle[0] = ((short)(result_buf[25]<<8 | result_buf[24]))/32768.0*180;   //X轴滚转角（x 轴）
			                    angle[1] = ((short)(result_buf[27]<<8 | result_buf[26]))/32768.0*180;   //Y轴俯仰角（y 轴）
			                    angle[2] = ((short)(result_buf[29]<<8 | result_buf[28]))/32768.0*180;   //Z轴偏航角（z 轴）
						        
							}
							else return;
						}
						else return;
					}
					else return;
				}
				else return;
			}
			else return;
		}
		else
		{				
			sign = 0;
			return;
		}
	}
	*/
}
