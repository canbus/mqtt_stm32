#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>

extern u8 rxBuf[];
extern u8 rxPtr;

int fputc(int ch, FILE* stream)
{

		USART_SendData(USART1, (uint8_t)ch);
		//等待数据发送完毕
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return ch;
}


void uart_test(void )
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA , &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



    //将结构体设置为缺省状态
    USART_StructInit(&USART_InitStructure);
    //波特率设置为9600
    USART_InitStructure.USART_BaudRate = 115200;
    //一帧数据的宽度设置为8bits
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    //在帧结尾传输1个停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    //奇偶失能模式，无奇偶校验
    USART_InitStructure.USART_Parity = USART_Parity_No;
    //发送/接收使能
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    //硬件流控制失能
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    //写入配置
    USART_Init(USART1, &USART_InitStructure);

    //打开串口1的中断响应函数，接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART1, USART_IT_TC, ENABLE);   //TC:一个字节传送完成中断
    //USART_ITConfig(USART1,USART_IT_TXE,DISABLE); //TXE:在数据寄存器为空的时候就会置一
    //打开串口1
    USART_Cmd(USART1, ENABLE);



    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		//选择中断分组2

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		//选择串口1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		//抢占式中断优先级设置为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		//响应式中断优先级设置为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //使能中断
    NVIC_Init(&NVIC_InitStructure);


    USART_ClearFlag(USART1, USART_FLAG_TC);
//    USART_SendData(USART1, 'H');
//		//等待数据发送完毕
//    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}


void USART1_IRQHandler(void)
{
    FlagStatus rxStatus, txStatus;
    //获得发送接收的状态
    rxStatus = USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
    txStatus = USART_GetFlagStatus(USART1, USART_FLAG_TC);
    //接收到数据
    if(rxStatus == SET)
    {
        unsigned char tmp = USART_ReceiveData(USART1);
				rxBuf[rxPtr++] = tmp;
				if(rxPtr >= 200)
					rxPtr = 0;
//        USART_SendData(USART1, tmp);
//				//等待数据发送完毕
//        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
    if(txStatus == SET)
    {
        USART_ClearFlag(USART1, USART_FLAG_TC);
    }
}
