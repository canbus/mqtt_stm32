
#include "gpio.h"

void initGPIO()
{
	GPIO_InitTypeDef initDef;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	initDef.GPIO_Pin = GPIO_Pin_11;
	initDef.GPIO_Mode = GPIO_Mode_Out_PP;
	initDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&initDef);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
}


void initGPIOReg()
{
//    RCC->APB2ENR|=1<<2;       //ʹ��PORTAʱ��
//    GPIOA->CRL&=0XFFFFFFF0;  
////    GPIOA->CRL|=0X00000008;  //PA0���ó�����  CNF1,CNF0 RM008_CN P113
//		GPIOA->CRL|=0X00000002;  //PA0���ó����  CNF1,CNF0 RM008_CN P113
//	
//		GPIOA->BSRR = 0x00000001;	//PA0 ��1
//		GPIOA->BRR = 0x00000001;	//PA0 ��0
//	
//    GPIOA->CRH&=0X0F0FFFFF;  //PA13,15���ó�����      
//    GPIOA->CRH|=0X80800000;                   
//    GPIOA->ODR|=1<<13;       //PA13����,PA0Ĭ������
//    GPIOA->ODR|=1<<15;       //PA15����
	
		RCC->APB2ENR|=RCC_APB2Periph_GPIOB;       //ʹ��PORTBʱ��
    GPIOB->CRL&=0XFFFFFFF0;  
//    GPIOB->CRL|=0X00000008;  //PB0���ó�����  CNF1,CNF0 RM008_CN P113
		GPIOB->CRL|=0X00000002;  //PB0���ó����  CNF1,CNF0 RM008_CN P113
	
		GPIOB->BSRR = 0x00000001;	//PB0 ��1
		GPIOB->BRR = 0x00000001;	//PB0 ��0
	
    GPIOB->CRH&=0X0F0FFFFF;  //PB13,15���ó�����      
    GPIOB->CRH|=0X80800000;                   
    GPIOB->ODR|=1<<13;       //PB13����,Ĭ������
    GPIOB->ODR|=1<<15;       //PB15����
}

