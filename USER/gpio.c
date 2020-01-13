
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
//    RCC->APB2ENR|=1<<2;       //使能PORTA时钟
//    GPIOA->CRL&=0XFFFFFFF0;  
////    GPIOA->CRL|=0X00000008;  //PA0设置成输入  CNF1,CNF0 RM008_CN P113
//		GPIOA->CRL|=0X00000002;  //PA0设置成输出  CNF1,CNF0 RM008_CN P113
//	
//		GPIOA->BSRR = 0x00000001;	//PA0 置1
//		GPIOA->BRR = 0x00000001;	//PA0 置0
//	
//    GPIOA->CRH&=0X0F0FFFFF;  //PA13,15设置成输入      
//    GPIOA->CRH|=0X80800000;                   
//    GPIOA->ODR|=1<<13;       //PA13上拉,PA0默认下拉
//    GPIOA->ODR|=1<<15;       //PA15上拉
	
		RCC->APB2ENR|=RCC_APB2Periph_GPIOB;       //使能PORTB时钟
    GPIOB->CRL&=0XFFFFFFF0;  
//    GPIOB->CRL|=0X00000008;  //PB0设置成输入  CNF1,CNF0 RM008_CN P113
		GPIOB->CRL|=0X00000002;  //PB0设置成输出  CNF1,CNF0 RM008_CN P113
	
		GPIOB->BSRR = 0x00000001;	//PB0 置1
		GPIOB->BRR = 0x00000001;	//PB0 置0
	
    GPIOB->CRH&=0X0F0FFFFF;  //PB13,15设置成输入      
    GPIOB->CRH|=0X80800000;                   
    GPIOB->ODR|=1<<13;       //PB13上拉,默认下拉
    GPIOB->ODR|=1<<15;       //PB15上拉
}

