#include "stm32f10x.h"
#include "string.h"
#include "mqtt.h"

extern void uartSend(u8 *p);
extern void delayMs(u16 nms);
extern void LEDON();
extern void LEDOFF();

extern u32 sysClk;

u8 idleTime = 0;
u32 timeOut = 0;
u8 rxBuf[200];
u8 rxPtr = 0;
static u8 devOnline = 0;
static u8 status = 0;

//p == NULL,���ȫ��,���������pָʾ��λ��
void clearMsg(u8 *p)
{
	if(p == NULL)
	{
		memset((void *)rxBuf,0,sizeof(rxBuf));
		rxPtr = 0;
	}
	else{
		char *p1 = strstr((const char *)rxBuf,p);
		u8 len = strlen(p1);
		//p1 += len;
		strcpy(rxBuf,p1);
		memset((void *)&rxBuf[len],0,sizeof(rxBuf)-len);
		rxPtr -=len;
	}
		
}
void uartSendHex(u8 *p,u8 len)
{
	clearMsg(NULL);
	
	while(len-->0)
	{
		USART_SendData(USART1, *p++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		//�ȴ����ݷ������
    //while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}
void uartSend(u8 *p)
{
	clearMsg(NULL);
	
	while(*p)
	{
		USART_SendData(USART1, *p++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		//�ȴ����ݷ������
    //while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}

s8 queryHexStr(unsigned char *buf,unsigned char *target,unsigned char len)
{
	for(;len>0;len--){
		if(*buf++ != *target++)
			return -1;
	}
	return 1;
}
//��rxBuf�в����Ƿ���ָ�����ַ���
//�ҵ�����1,û�ҵ�����0,û�յ����ݷ���-1
s8 queryString(char *p)
{
	if(rxPtr > 0){
		if(strstr((const char *)rxBuf,p) != 0){
//			rxPtr = 0;
//			memset((void *)rxBuf,0,sizeof(rxBuf));
			return 1;
		}else if(strstr((const char *)rxBuf,"ERROR") != 0){
			return -1;
		}else {
			return 0;
		}
	}
	return -1;
}
s8 queryString2(char *p1,char *p2)
{
	if(rxPtr > 0)
	{
		if(strstr((const char *)rxBuf,p1) != 0)
		{
			if(strstr((const char *)rxBuf,p2) != 0)
			{
//				rxPtr = 0;
//				memset((void *)rxBuf,0,sizeof(rxBuf));
				return 1;
			}
		}
		return 0;
	}
	return -1;
}



s8 waitAck(char *p,u32 timeOut)
{
	u32 t = sysClk + timeOut;
	u8 oldPtr = 0;
	while(1){
		if(sysClk > t)
			return -1; //��ʱ
		if(rxPtr > 0){
			while(1){	//�ȴ��������
				if(sysClk > t)
					return -1; //��ʱ
				oldPtr = rxPtr;
				delayMs(1); 
				//if(queryString("\r\n"))
				if(oldPtr == rxPtr) //û���µ����ݵ���
					break;
			}
			if(queryString(p) == 1)
				return 1;
		}
	}
}
void initWIFI()
{
	uartSend("AT+RST\r\n");
	waitAck("ready",5 * 1000);
	uartSend("AT+CWMODE=1\r\n");
	waitAck("OK",5 * 100);
	uartSend("AT+CWJAP=\"Qwer\",\"b12345678\"\r\n");
	waitAck("WIFI GOT IP",10 * 1000);
}
void sendMsg(unsigned char *p) //���ͳ��Ȳ��ܳ���99
{
	u8 len = strlen(p);
	u8 buf[]="AT+CIPSEND=99\r\n";
	if(len > 10)
	{
		buf[12] = len % 10 + 0x30;
		buf[11] = len / 10 + 0x30;
	}else{
		buf[11] = len % 10 + 0x30;
		buf[12] = '\r';buf[13] = '\n';buf[14] = 0;
	}		
	uartSend(buf);
	waitAck(">",1000);
	uartSend(p);
}
void sendMsgHex(unsigned char *p,unsigned char len) //���ͳ��Ȳ��ܳ���99
{
	//u8 len = strlen(p);
	u8 buf[]="AT+CIPSEND=99\r\n";
	if(len > 10)
	{
		buf[12] = len % 10 + 0x30;
		buf[11] = len / 10 + 0x30;
	}else{
		buf[11] = len % 10 + 0x30;
		buf[12] = '\r';buf[13] = '\n';buf[14] = 0;
	}		
	uartSend(buf);
	waitAck(">",1000);
	uartSendHex(p,len);
}
void do8266()
{
	s8 result = 0;
	const char* addr = "a1xzbagTSmy.iot-as-mqtt.cn-shanghai.aliyuncs.com";
	const char* port = "1883";
	switch(status)
	{
		case 0:
			uartSend("AT+CIFSR\r\n");
			if(waitAck("busy",100) == 1)
			{	delayMs(2000);uartSend("AT+CIFSR\r\n");}
			if(waitAck("CIFSR:STAIP,\"1",5 * 1000) == 1)
				status = 1;
			else 
				initWIFI();
//			delayMs(500);
//			result = queryString("CIFSR:STAIP,\"0.0.0.0\"");
//			if(result == 1 || result == -1)
//				initWIFI();
//			else{
//				status = 1;
//			}
			break;
		case 1: //����WIFI
			uartSend("AT+CIPMUX=0\r\n");
			waitAck("OK",5 * 100);
			//uartSend("AT+CIPSTART=\"TCP\",\"117.78.1.201\",8800\r\n");
			uartSend("AT+CIPSTART=\"TCP\",\"192.168.43.195\",1883\r\n");
			status = 2;
			timeOut = sysClk + 5*1000;//5�볬ʱ
			break;
		case 2:
			if(sysClk > timeOut){ //���ӳ�ʱ
				if(devOnline==1)
					uartSend("AT+CIPCLOSE=1\r\n");
				status = 1;
			}
			if(queryString("CONNECT") > 0){
				u8 buf[200]={0},len= 0;
				devOnline = 1;
				len = GetDataConnet(buf);
				sendMsgHex(buf,len); //���� 20 02 00 00
				waitAck("+IPD,",5*1000);
				{
					char *t = "+IPD,",*p;
					p = strstr(rxBuf,t);
					p += strlen(t);
					p += 2;
					if(queryHexStr(p,"\x20\x02\x00\x00",4) == 1)
					{	status = 3;clearMsg(NULL);}
				}
			}
			break;
		case 3: //��¼�ɹ�,
			{		//��������
				u8 buf[200];
				u8 *topic="/topic/#";
				u8 msgID = 100;
				u8 len;
				len = GetDataSUBSCRIBE(buf,(const char *)topic,msgID,0);
				sendMsgHex(buf,len); 
				waitAck("+IPD,",5*1000);
				{
					char *t = "+IPD,",*p;
					p = strstr(rxBuf,t);
					p += strlen(t);
					p += 2;
					if(queryHexStr(p,"\x90",1) == 1)
						status = 4;			//���ĳɹ�
				}
			}
			break;
		case 4: //������Ϣ
			{
				u8 buf[200];
				u8 *msg="hello from stm32";
				u8 *topic="/topic";
				u8 len;
				len = GetDataPUBLISH(buf,0,0,0,(const char *)topic,(const char *)msg);
				sendMsgHex(buf,len); 
			}
			timeOut = sysClk + 5*1000;//5�볬ʱ
			status = 5;
			break;
		case 5://�ȴ���Ϣ���ͳɹ�
				if(sysClk > timeOut){ //��ʱ,�ط�
					status = 3;
				}
				if(queryString("SEND OK") > 0){ //������Ϣ�ɹ�
					clearMsg(NULL);
					status = 6;
				}
			break;
		case 6://IDLE
			idleTime ++;
			if(idleTime > 30){ //���ʱ1����,30�뷢��һ������
				u8 buf[2];
				clearMsg(NULL);
				GetDataPINGREQ(buf);
				sendMsgHex(buf,2);
				waitAck("+IPD,",1*1000);
				{
					char *t = "+IPD,",*p;
					p = strstr(rxBuf,t);
					p += strlen(t);
					p += 2;
					if(queryHexStr(p,"\xD0\x00",2) == 1){
						clearMsg(NULL);
						idleTime = 0;
						
						status = 4;//����һ����Ϣ
					}
				}
			}
			if(idleTime > 60 * 2) //���ӿ����Ѿ��Ͽ�
			{
				status = 0;
			}
			if(queryString("+IPD,") > 0){ //�յ���������Ϣ
					char *t = "+IPD,",*p;
					char msgLen;
					char topicLen;
					char msg[200];
					p = strstr(rxBuf,t);
				  p = strstr(p,":");
					p += 2;
					
					msgLen = *p++;
					p++;
					topicLen = *p;
					strncpy(msg,p+topicLen,msgLen-topicLen);
					if(strstr(p,"OFF"))
						LEDOFF();
					else
						LEDON();
				
					status = 6;
				clearMsg(NULL);
			}
			break;
	}
}
