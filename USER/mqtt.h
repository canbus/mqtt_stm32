#ifndef __MQTT_H

#define __MQTT_H

//#include "sys.h"
#include <string.h>

#define MQTT_TypeCONNECT 1//请求连接
#define MQTT_TypeCONNACK 2//请求应答
#define MQTT_TypePUBLISH 3//发布消息
#define MQTT_TypePUBACK 4//发布应答
#define MQTT_TypePUBREC 5//发布已接收，保证传递1
#define MQTT_TypePUBREL 6//发布释放，保证传递2
#define MQTT_TypePUBCOMP 7//发布完成，保证传递3
#define MQTT_TypeSUBSCRIBE 8//订阅请求
#define MQTT_TypeSUBACK 9//订阅应答
#define MQTT_TypeUNSUBSCRIBE 10//取消订阅
#define MQTT_TypeUNSUBACK 11//取消订阅应答
#define MQTT_TypePINGREQ 12//ping请求
#define MQTT_TypePINGRESP 13//ping响应
#define MQTT_TypeDISCONNECT 14//断开连接
#define MQTT_StaCleanSession 1 //清理会话
#define MQTT_StaWillFlag 0 //遗嘱标志
#define MQTT_StaWillQoS 0 //遗嘱QoS连接标志的第4和第3位。
#define MQTT_StaWillRetain 0 //遗嘱保留
#define MQTT_StaUserNameFlag 1 //用户名标志 User Name Flag
#define MQTT_StaPasswordFlag 1 //密码标志 Password Flag
#define MQTT_KeepAlive 60
#define MQTT_ClientIdentifier "Tan1" //客户端标识符 Client Identifier
#define MQTT_WillTopic "" //遗嘱主题 Will Topic
#define MQTT_WillMessage "" //遗嘱消息 Will Message
#define MQTT_UserName "admin" //用户名 User Name
#define MQTT_Password "password" //密码 Password
 

/***
*获取发布消息的数据包
*dup(0):[0,1]重发标志:0--第一次发送,1--重发的数据包
*qos(0):[0-2]
*retain(0):服务器是否保存该消息
****/
unsigned char GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);

/*
* 订阅主题的数据包 MsgID:序号 RequestedQoS:服务质量要求0,1或2
*	成功返回90  MsgID RequestedQoS
*/
unsigned char GetDataSUBSCRIBE(unsigned char *buff,const char *subTopic,unsigned int MsgID,unsigned char RequestedQoS);

void GetDataDisConnet(unsigned char *buff);//获取断开连接的数据包
unsigned char GetDataConnet(unsigned char *buff);//获取连接的数据包正确连接返回20 02 00 00
unsigned char GetDataPINGREQ(unsigned char *buff);//心跳请求的数据包成功返回d0 00

#endif
