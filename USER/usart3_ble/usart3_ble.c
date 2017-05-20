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

/*********************************************************************************
*********************MCU���� STM32F407Ӧ�ÿ�����(��ǿ��)**************************
**********************************************************************************
* �ļ�����: lte.c                                                                *
* �ļ�����������ʹ��                                                             *
* �������ڣ�2015.10.06                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ���������ֻ�APP���ƿ�����                                                * 
**********************************************************************************
*********************************************************************************/	

u8 receive_str[UART3_REC_NUM];     //���ջ�������,���USART_REC_LEN���ֽ� 
u8 send_str[UART3_TRS_NUM]; 
u8 uart_byte_count=0;
u8 APP_mode=0;          //APP����ģʽ  0�����������  1�����շ�����

//float voice = 0.0f;
//float dis1 = 0.0f;
/****************************************************************************
* ��    ��: void LTE_uart4_init(u32 bound)
* ��    �ܣ�LTE_uart4��ʼ��
* ��ڲ�����bound��������   
* ���ز�������
* ˵    ���� 
****************************************************************************/
void LTE_uart3_init(u32 bound)
{   
/****************************** ����3��ʼ��*********************************/
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ�� 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //ʹ��USART3ʱ�� 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);  //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
	//USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10��GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);          //��ʼ��PB10��PB11
   //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure); //��ʼ������1	
    USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);         //��������ж�
	//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;      //����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		   //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	  //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
/****************************** ����3��ʼ��**********************************/

/******************************* ����IO�ڳ�ʼ��******************************/
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    //GPIOA7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;             //��ͨ����ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;       //100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;          
	GPIO_Init(GPIOG,&GPIO_InitStructure);          //��ʼ��PA9��PA10
}

//����1����һ���ַ�
void uart3SendChar(u8 ch)
{      
	while((USART3->SR&0x40)==0);  
    USART3->DR = (u8) ch;      
}
/****************************************************************************
* ��    ��: void uart1SendChars(u8 *str, u16 strlen)
* ��    �ܣ�����1����һ�ַ���
* ��ڲ�����*str�����͵��ַ���
            strlen���ַ�������
* ���ز�������
* ˵    ���� 
****************************************************************************/
void uart3SendChars(u8 *str, u16 strlen)
{ 
	u16 k= 0 ; 
   	do 
   	{ 
   		uart3SendChar(*(str + k)); k++; 
   	}while (k < strlen); //ѭ������,ֱ���������  
} 

__align(8) u8  send_str[UART3_TRS_NUM];
void u3_printf(char* fmt,...)  
{  
	int cnt = 0;
	int len = 0;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)send_str,fmt,ap);//��ap����fmt��ʽд�봮��3������
	va_end(ap);//������λ�ȡ 
	len = strlen((const char*)send_str);
	while (len--)
	{
		while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == 0);
		USART_SendData(USART3,send_str[cnt++]);
	}                        
}

//����1�жϷ������
void USART3_IRQHandler(void)  
{
	u8 rec_data;
	int len = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж� 
	{
		rec_data =(u8)USART_ReceiveData(USART3);         //(USART1->DR) ��ȡ���յ�������
        if(rec_data=='S')		  	                         //�����S����ʾ��������Ϣ����ʼλ
		{
			uart_byte_count=0x01; 
		}

		else if(rec_data=='E')		                         //���E����ʾ��������Ϣ���͵Ľ���λ
		{
			if(strcmp("open_led1",(char *)receive_str)==0)         LED0=0;	   //����LED1
			else if(strcmp("close_led1",(char *)receive_str)==0)   LED0=1;	   //����LED1
			else if(strcmp("open_led2",(char *)receive_str)==0)    LED1=0;	   //����LED1
			else if(strcmp("close_led2",(char *)receive_str)==0)   LED1=1;	   //����LED1
			else if(strcmp("open_led3",(char *)receive_str)==0)    LED2=0;	   //����LED1
			else if(strcmp("close_led3",(char *)receive_str)==0)   LED2=1;	   //����LED1
			else if(strcmp("open_beep",(char *)receive_str)==0)    BEEP=1; 	   //��������
			else if(strcmp("close_beep",(char *)receive_str)==0)   BEEP=0; 	   //����������
			else if(strcmp("test",(char *)receive_str)==0)
			{
				uart3SendChars("correct_OKcorrect_OKcorrect_OK",30);
			}
			else if(strcmp("Data",(char *)receive_str)==0)
			{
				//............test
				len = strlen((const char*)package_data);

				//����1���Է�
				uart1SendChars(package_data,len);
				//uart3SendChars(package_data,len);
			}
			else if(strcmp("send_acc",(char *)receive_str)==0)
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
			else if(strcmp("send_pal",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",w[0],"��/s, ");
				u3_printf("%.3f%s",w[1],"��/s, ");
				u3_printf("%.3f%s",w[2],"��/s;");
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
			else if(strcmp("send_ang",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",angle[0],"��, ");
				u3_printf("%.3f%s",angle[1],"��, ");
				u3_printf("%.3f%s",angle[2],"��;");
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
			else if(strcmp("send_tem",(char *)receive_str)==0)
			{
				LED2=!LED2;//����
				/*
				u3_printf("%.3f%s",T,"��;");
			    USART_SendData(USART3,0x1a);
			    */
			    sprintf(send_str, "%.3f%s",T,"C");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("send_dis1",(char *)receive_str)==0)
			{
				LED2=!LED2;//����
				
			  sprintf(send_str, "%.3f%s",dis[0],"cm");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("send_voi",(char *)receive_str)==0)
			{
				/*
				u3_printf("%.3f%s",T,"��;");
			    USART_SendData(USART3,0x1a);
			    */
				voice = (float)Get_Adc(ADC_Channel_4) / 4096.0f * 430.0 / 5.0;
			  sprintf(send_str, "%.3f%s",voice,"dB");
				len = strlen((const char*)send_str);
				uart3SendChars(send_str,len);
			}
			else if(strcmp("app_mode1",(char *)receive_str)==0)    
		    {
				APP_mode=0; 
		        LCD_DisplayString(20,130,16,"APP_mode: 0  ");
            	LCD_DisplayString(20,150,16,"Wait APP Control");
				LCD_Fill_onecolor(0,170,239,319,WHITE);
		    }//APPΪ״̬������
			else if(strcmp("app_mode2",(char *)receive_str)==0)    
		    {
				APP_mode=1;
				LCD_DisplayString(20,130,16,"APP_mode: 1  ");
                LCD_DisplayString(20,150,16,"Receive and send");
				LCD_Fill_onecolor(0,170,239,319,WHITE);
		    }//APPΪ���շ�����
			
			else if(receive_str[0]=='T')   //ʱ��У׼
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
		    if(APP_mode==1)   //APPΪ���շ�����ʱ����ʾAPP����������
			{
			    LCD_DisplayString(30,190,16,receive_str);	 //���Ը����Լ����͵����� д��Ӧ�Ķ��� ��ǰ��ı�д����
			}
				
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

