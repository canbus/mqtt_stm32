#include "stm32f10x.h"
#include "string.h"

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
	while(1){
		if(sysClk > t)
			return -1; //��ʱ
		if(rxPtr > 0){
			while(1){	//�ȴ��������
				if(sysClk > t)
					return -1; //��ʱ
				delayMs(1); 
				if(queryString("\r\n"))
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
void sendMsg(char *p) //���ͳ��Ȳ��ܳ���99
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
void do8266()
{
	s8 result = 0;
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
			uartSend("AT+CIPSTART=\"TCP\",\"117.78.1.201\",8800\r\n");
			status = 2;
			timeOut = sysClk + 5*1000;//5�볬ʱ
			break;
		case 2:
			if(sysClk > timeOut){ //���ӳ�ʱ
				if(devOnline==1)
					uartSend("AT+CIPCLOSE=1\r\n");
				status = 0;
			}
			if(queryString("OK") > 0){
				devOnline = 1;
				sendMsg("{\"t\":1,\"device\":\"device01 \",\"key\":\"7e8022ced4f6459e879de53c6b5f4eef\",\"ver\":\"1.1\"}");
				delayMs(500*5);
				if(queryString2("\"status\":0","\"t\":2")){
					status = 3;
				}else{
					//status = 0;
				}
			}
			break;
		case 3: //��¼�ɹ�,������Ϣ
			sendMsg("{\"t\":3,\"datatype\":1,\"datas\":{\"temperature\":\"74\"},\"msgid\":\"74\"}");
			timeOut = sysClk + 5*1000;//5�볬ʱ
			status = 4;
//			delayMs(500*5);
//			if(queryString2("\"status\":0","\"t\":4")){ //������Ϣ�ɹ�
//					status = 3;
//				}else{
//					status = 0;
//				}
			break;
		case 4://�ȴ���Ϣ���ͳɹ�
				if(sysClk > timeOut){ //��ʱ,�ط�
					status = 3;
				}
				if(queryString2("\"status\":0","\"t\":4") > 0){ //������Ϣ�ɹ�
					clearMsg("\"t\":4");
					status = 5;
				}
			break;
		case 5://IDLE
			idleTime ++;
			if(idleTime > 30){ //���ʱ1����,30�뷢��һ������
				clearMsg(NULL);
				sendMsg("$#AT#");
				if(waitAck("$OK##",10)){
					clearMsg(NULL);
					idleTime = 0;
				}
			}
			if(queryString2("+IPD,","\"t\":5") > 0){ //�յ���������Ϣ
				char *targetStr = "\"data\":"; //"data":{"a":1,"b":2}
					char *p = strstr(rxBuf,targetStr);
					{
						char *aStr = "\"a\":"; //"a":
						char *a = strstr(p,aStr); 
						if(a != NULL){
							a += strlen(aStr);
							if(*a == '1')
								LEDON();
							else
								LEDOFF();
						}
					}
					{
						char *bStr = "\"b\":"; //"a":
						char *b = strstr(p,bStr); 
						if(b != NULL){
							b += strlen(bStr);
							if(*b == '1')
								LEDON();
							else
								LEDOFF();
						}
					}
//					p += strlen(targetStr);
//					p ++;
//					
//				if(*p == '1')  //�õ����� 
//						 LEDON();
//					else 
//						LEDOFF();
					status = 5;
				clearMsg(NULL);
			}
			break;
	}
}
