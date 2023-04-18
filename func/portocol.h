//
// Created by fumoumou on 23-3-20.
//

#ifndef TCPCLIENT_PORTOCOL_H
#define TCPCLIENT_PORTOCOL_H

#include "stdlib.h"
#include "unistd.h"
#include "string.h"

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed : name or pwd error or relogin"

#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend existed"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NOEXIST "usr not exist"

#define DEL_FRIEND_OK "delete friend ok"


enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUDEST,      /*注册请求*/
    ENUM_MSG_TYPE_REGIST_RESPOND,       /*注册回复*/

    ENUM_MSG_TYPE_LOGIN_REQUEST,        /*登录请求*/
    ENUM_MSG_TYPE_LOGIN_RESPOND,        /*登录回复*/

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   /*在线用户请求*/
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   /*在线用户回复*/

    ENUM_MSG_TYPE_SERACH_USR_REQUEST,   /*搜索用户请求*/
    ENUM_MSG_TYPE_SERACH_USR_RESPOND,   /*搜索用户回复*/

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   /*添加好友请求*/
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   /*添加好友回复*/

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,     /*添加好友同意*/
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,    /*添加好友拒绝*/

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST, /*刷新好友请求*/
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND, /*刷新好友回复*/

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, /*删除好友请求*/
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, /*删除好友回复*/

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST, /*私聊请求*/
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND, /*私聊回复*/

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST, /*群聊请求*/
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND, /*群聊回复*/

    ENUM_MSG_TYPE_MAX = 0x00ffffff
};

struct PDU{
    uint uiPDULen;       //协议单元大小
    uint uiMsgType;
    char caData[64];
    uint uiMsgLen;
    int caMsg[];
};

PDU *mkPDU(uint uiMsgLen);

#endif //TCPCLIENT_PORTOCOL_H
