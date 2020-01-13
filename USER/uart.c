#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>

extern u8 rxBuf[];
extern u8 rxPtr;

int fputc(int ch, FILE* stream)
{

		USART_SendData(USART1, (uint8_t)ch);
		//�ȴ����ݷ������
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



    //���ṹ������Ϊȱʡ״̬
    USART_StructInit(&USART_InitStructure);
    //����������Ϊ9600
    USART_InitStructure.USART_BaudRate = 115200;
    //һ֡���ݵĿ������Ϊ8bits
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    //��֡��β����1��ֹͣλ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    //��żʧ��ģʽ������żУ��
    USART_InitStructure.USART_Parity = USART_Parity_No;
    //����/����ʹ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    //Ӳ��������ʧ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    //д������
    USART_Init(USART1, &USART_InitStructure);

    //�򿪴���1���ж���Ӧ�����������ж�
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART1, USART_IT_TC, ENABLE);   //TC:һ���ֽڴ�������ж�
    //USART_ITConfig(USART1,USART_IT_TXE,DISABLE); //TXE:�����ݼĴ���Ϊ�յ�ʱ��ͻ���һ
    //�򿪴���1
    USART_Cmd(USART1, ENABLE);



    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		//ѡ���жϷ���2

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		//ѡ�񴮿�1�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		//��ռʽ�ж����ȼ�����Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		//��Ӧʽ�ж����ȼ�����Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //ʹ���ж�
    NVIC_Init(&NVIC_InitStructure);


    USART_ClearFlag(USART1, USART_FLAG_TC);
//    USART_SendData(USART1, 'H');
//		//�ȴ����ݷ������
//    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}


void USART1_IRQHandler(void)
{
    FlagStatus rxStatus, txStatus;
    //��÷��ͽ��յ�״̬
    rxStatus = USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
    txStatus = USART_GetFlagStatus(USART1, USART_FLAG_TC);
    //���յ�����
    if(rxStatus == SET)
    {
        unsigned char tmp = USART_ReceiveData(USART1);
				rxBuf[rxPtr++] = tmp;
				if(rxPtr >= 200)
					rxPtr = 0;
//        USART_SendData(USART1, tmp);
//				//�ȴ����ݷ������
//        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
    if(txStatus == SET)
    {
        USART_ClearFlag(USART1, USART_FLAG_TC);
    }
}
