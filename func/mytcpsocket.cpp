//
// Created by fumoumou on 23-3-20.
//

#include "mytcpsocket.h"
#include "portocol.h"
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket() {
    connect(this, SIGNAL(readyRead())
            , this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected())
            , this, SLOT(clientOffline()));
}

void MyTcpSocket::recvMsg() {

//    qDebug() << this->bytesAvailable();

    uint uiPDULen = 0;

    this->read((char *)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen-sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);                                        /*接收的pdu*/
    this->read((char *)pdu+sizeof(uint),uiPDULen);



    switch (pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGIST_REQUDEST:{
//            qDebug() <<"数据类型" << pdu->uiMsgType;
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            qDebug() << "用户名： " << caName << "密码： " << caPwd;
            bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);

            PDU *respdu = mkPDU(0);                                     /*回复的pdu*/

            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if (ret){
                strcpy(respdu->caData,REGIST_OK);

                qDebug() << "注册成功";

            } else{
                strcpy(respdu->caData,REGIST_FAILED);
                qDebug() << "注册失败";
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            pdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:{
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            qDebug() << "用户名： " << caName << "密码： " << caPwd;
            bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);

            PDU *respdu = mkPDU(0);                                     /*回复的pdu*/

            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if (ret){
                strcpy(respdu->caData,LOGIN_OK);

                qDebug() << "登录成功";
                m_strName = caName;

            } else{
                strcpy(respdu->caData,LOGIN_FAILED);
                qDebug() << "登录失败";
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            pdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:{

            QStringList ret = OpeDB::getInstance().handleAllOnline();
            qDebug() << "返回所有在线用户";

            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiPDULen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for (int i = 0; i < ret.size(); ++i) {
                memcpy((char*)(respdu->caMsg)+i*32,
                       ret.at(i).toStdString().c_str(),
                       ret.at(i).size());
            }
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SERACH_USR_REQUEST:{
            int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SERACH_USR_RESPOND;
            if(-1 == ret){
                strcpy(respdu->caData,SEARCH_USR_NO);
            } else if(1 == ret){
                strcpy(respdu->caData,SEARCH_USR_ONLINE);
            }else if(0 == ret){
                strcpy(respdu->caData,SEARCH_USR_OFFLINE);
            }

            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            int ret = OpeDB::getInstance().handleAddFriend(caPerName,caName);
            PDU *respdu = NULL;

            qDebug() << ret;

            switch (ret) {
                case -1:{
                    qDebug() << "名称空";
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,UNKNOW_ERROR);
                    write((char *)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                    break;
                }
                case 0:{
                    qDebug() << "好友已经存在";
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,EXISTED_FRIEND);
                    write((char *)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                    break;
                }
                case 1:{
                    qDebug() << "符合添加好友条件，准备添加好友，返回原pdu";
                    mytcpserver::getInstance().resend(caPerName,pdu);      /*返回原来的pdu*/
                    break;
                }
                case 2:{
                    qDebug() << "好友不在线";
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
                    write((char *)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                    break;
                }
                case 3:{
                    qDebug() << "用户不存在";
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,ADD_FRIEND_NOEXIST);
                    write((char *)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                    break;
                }
            }

            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:{
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            qDebug() << "同意添加好友" << "perName" << caPerName << "name" << caName;
            OpeDB::getInstance().handleAddFriendToSQL(caPerName,caName);
            PDU *respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:{

            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            qDebug() << caName << "拒绝加" << caPerName << "为好友";
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for (int i = 0; i < ret.size(); ++i) {
                memcpy((char *)(respdu->caMsg)+i*32
                ,ret.at(i).toStdString().c_str()
                ,ret.at(i).size());
            }
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
            char strSelfName[32] = {'\0'};
            char strFriendName[32] = {'\0'};
            strncpy(strSelfName,pdu->caData,32);
            strncpy(strFriendName,pdu->caData+32,32);
            OpeDB::getInstance().handleDelFriend(strSelfName,strFriendName);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            strcpy(respdu->caData,DEL_FRIEND_OK);

            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            mytcpserver::getInstance().resend(strFriendName,pdu);      /*返回原来的pdu*/

            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
            char caPerName[32] = {'\0'};
            memcpy(caPerName,pdu->caData+32,32);
            qDebug() << caPerName;
            mytcpserver::getInstance().resend(caPerName,pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName);
            QString tmp;
            for (int i = 0; i < onlineFriend.size(); ++i) {
                tmp = onlineFriend.at(i);
                mytcpserver::getInstance().resend(tmp.toStdString().c_str(),pdu);
                qDebug() << "转发到：" << tmp;
            }


            break;
        }
        default:{
            break;
        }

    }
    free(pdu);
    pdu = NULL;

}

void MyTcpSocket::clientOffline() {
    qDebug() << "客户端下线，用户名：" << m_strName;
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);

}


QString MyTcpSocket::getName() {
    return m_strName;
}

