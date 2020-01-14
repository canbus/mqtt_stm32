#ifndef __MQTT_H

#define __MQTT_H

//#include "sys.h"
#include <string.h>

#define MQTT_TypeCONNECT 1//��������
#define MQTT_TypeCONNACK 2//����Ӧ��
#define MQTT_TypePUBLISH 3//������Ϣ
#define MQTT_TypePUBACK 4//����Ӧ��
#define MQTT_TypePUBREC 5//�����ѽ��գ���֤����1
#define MQTT_TypePUBREL 6//�����ͷţ���֤����2
#define MQTT_TypePUBCOMP 7//������ɣ���֤����3
#define MQTT_TypeSUBSCRIBE 8//��������
#define MQTT_TypeSUBACK 9//����Ӧ��
#define MQTT_TypeUNSUBSCRIBE 10//ȡ������
#define MQTT_TypeUNSUBACK 11//ȡ������Ӧ��
#define MQTT_TypePINGREQ 12//ping����
#define MQTT_TypePINGRESP 13//ping��Ӧ
#define MQTT_TypeDISCONNECT 14//�Ͽ�����
#define MQTT_StaCleanSession 1 //����Ự
#define MQTT_StaWillFlag 0 //������־
#define MQTT_StaWillQoS 0 //����QoS���ӱ�־�ĵ�4�͵�3λ��
#define MQTT_StaWillRetain 0 //��������
#define MQTT_StaUserNameFlag 1 //�û�����־ User Name Flag
#define MQTT_StaPasswordFlag 1 //�����־ Password Flag
#define MQTT_KeepAlive 60
#define MQTT_ClientIdentifier "Tan1" //�ͻ��˱�ʶ�� Client Identifier
#define MQTT_WillTopic "" //�������� Will Topic
#define MQTT_WillMessage "" //������Ϣ Will Message
#define MQTT_UserName "admin" //�û��� User Name
#define MQTT_Password "password" //���� Password
 

/***
*��ȡ������Ϣ�����ݰ�
*dup(0):[0,1]�ط���־:0--��һ�η���,1--�ط������ݰ�
*qos(0):[0-2]
*retain(0):�������Ƿ񱣴����Ϣ
****/
unsigned char GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);

/*
* ������������ݰ� MsgID:��� RequestedQoS:��������Ҫ��0,1��2
*	�ɹ�����90  MsgID RequestedQoS
*/
unsigned char GetDataSUBSCRIBE(unsigned char *buff,const char *subTopic,unsigned int MsgID,unsigned char RequestedQoS);

void GetDataDisConnet(unsigned char *buff);//��ȡ�Ͽ����ӵ����ݰ�
unsigned char GetDataConnet(unsigned char *buff);//��ȡ���ӵ����ݰ���ȷ���ӷ���20 02 00 00
unsigned char GetDataPINGREQ(unsigned char *buff);//������������ݰ��ɹ�����d0 00

#endif
