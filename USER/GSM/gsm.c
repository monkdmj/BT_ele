#include "gsm.h"
#include "string.h"
#include "stdarg.h"
#include "led.h"

u8  UART4_RX_BUF[UART4_REC_LEN]; //RECEIVE
u8  UART4_TX_BUF[UART4_TRS_LEN]; //SEND
u32 u4_count = 0;

void UART4_Config(void)   //初始化 配置UART4
{
	GPIO_InitTypeDef     GPIO_InitStructure;   //串口端口配置结构体变量
	USART_InitTypeDef    USART_InitStructure;  //串口参数配置结构体变量
	NVIC_InitTypeDef     NVIC_InitStructure;   //中断结构体变量

	//使能 UART4 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//打开串口复用时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);   //打开PD端口时钟
	
  	//!!!!!!!!!!!!!!!!!!!!!!!!GPIOA!!!!!!!!!!!!!!
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);//
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4);//
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;					   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		   //复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	   //输出速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   //初始化GPIOA
	  
	//配置UART4参数
	USART_InitStructure.USART_BaudRate = 115200;	                    //波特率设置：115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(UART4, &USART_InitStructure);                       //初始化UART4
	//USART_ITConfig(UART4, USART_IT_TXE, ENABLE);  // 使能指定的UART4发送中断
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // 使能指定的UART4接收中断
	//使能 USART2， 配置完毕
	USART_Cmd(UART4, ENABLE);                             // UART4使能
	USART_ClearFlag(UART4, USART_FLAG_TC);                //清串口4发送标志
	
	
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_Init(&NVIC_InitStructure);
}
/*
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
*/
/////GSM测试部分代码
//发送命令函数

__align(8) u8  UART4_TX_BUF[UART4_TRS_LEN];
void u4_printf(char* fmt,...)  
{  
	int cnt = 0;
	int len = 0;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap);//将ap按照fmt格式写入串口3缓存区
	va_end(ap);//结束变参获取 
	len = strlen((const char*)UART4_TX_BUF);
	while (len--)
	{
		while (USART_GetFlagStatus(UART4,USART_FLAG_TC) == 0);
		USART_SendData(UART4,UART4_TX_BUF[cnt++]);
	}                        
}

void GSM_START(char* cmd , int timeout)//启动指令
{
	int time=0;  
	u4_printf("%s\r\n",cmd);                   //发送指令
	while(time < timeout)
	{
		if(strstr( (const char *) UART4_RX_BUF , (const char *)"OK"))//如果接受到OK表示指令成功
		{                     
			memset(UART4_RX_BUF,0,UART4_REC_LEN);                  //清除串口3数据	
			u4_count = 0;
			/*
			GPIO_SetBits(GPIOC,GPIO_Pin_1);
			delay_ms(50);
			GPIO_ResetBits(GPIOC,GPIO_Pin_1);
			delay_ms(50);
			*/
			LED2=!LED2;
			break;
		}
		else if (strstr((const char*) UART4_RX_BUF , (const char*) "ERROR"))//如果接受到ERROR，说明出错
		{
			u4_printf("%s\r\n",cmd);   //重新发送命令，从头开始该命令的执行
			time = 0;
			u4_count = 0;
			memset(UART4_RX_BUF,0,UART4_REC_LEN);
		}
		time ++;     
	}
}
void GSM_SendAccData( float data1 , float data2 , float data3)//加速度
{
	u4_printf("%s\r\n","AT+CIPSEND");
	delay_ms(200);
	if ( strstr ((const char*) UART4_RX_BUF , (const char*) "ERROR") )//返回错误时重新发送指令
		{
			memset(UART4_RX_BUF,0,UART4_REC_LEN);
		  u4_printf("%s\r\n","AT+CIPSEND");
	    delay_ms(200);
		}
		u4_printf("%.3f%s",data1,"m/s2, ");
		u4_printf("%.3f%s",data2,"m/s2, ");
		u4_printf("%.3f%s",data3,"m/s2, ");
	  USART_SendData(UART4,0x1a);
		delay_ms(200);
}
void GSM_SendPalData( float data1 , float data2 , float data3)//角速度
{
	u4_printf("%s\r\n","AT+CIPSEND");
	delay_ms(200);
	if ( strstr ((const char*) UART4_RX_BUF , (const char*) "ERROR") )//返回错误时重新发送指令
		{
			memset(UART4_RX_BUF,0,UART4_REC_LEN);
		  u4_printf("%s\r\n","AT+CIPSEND");
	    delay_ms(200);
		}
		u4_printf("%.3f%s",data1,"度/s, ");
		u4_printf("%.3f%s",data2,"度/s, ");
		u4_printf("%.3f%s",data3,"度/s, ");
	  USART_SendData(UART4,0x1a);
		delay_ms(200);
}
void GSM_SendAngData( float data1 , float data2 , float data3)//角度
{
	u4_printf("%s\r\n","AT+CIPSEND");
	delay_ms(200);
	if ( strstr ((const char*) UART4_RX_BUF , (const char*) "ERROR") )//返回错误时重新发送指令
		{
			memset(UART4_RX_BUF,0,UART4_REC_LEN);
		  u4_printf("%s\r\n","AT+CIPSEND");
	    delay_ms(200);
		}
		u4_printf("%.3f%s",data1,"度, ");
		u4_printf("%.3f%s",data2,"度, ");
		u4_printf("%.3f%s",data3,"度, ");
	  USART_SendData(UART4,0x1a);
		delay_ms(200);
}
void GSM_Init(void)
{
	UART4_Config();
	//GPIO_Config();

	big_delay(5000);
	GSM_START("AT+CGATT=1",5);
	big_delay(2000);
	GSM_START("AT+CGACT=1,1",5);
	big_delay(2000);
	GSM_START("AT+CIPSTART=TCP,223.3.48.150,8266",20);  //118.26.119.118,8266
	big_delay(2000);
}
void UART4_IRQHandler(void)
{
	//LED2=!LED2;
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		//LED2=!LED2;
		UART4_RX_BUF[u4_count] = USART_ReceiveData(UART4);
	    u4_count ++;
	}
}
