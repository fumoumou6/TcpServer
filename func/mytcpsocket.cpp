//
// Created by fumoumou on 23-3-20.
//

#include "mytcpsocket.h"
#include "portocol.h"
#include "mytcpserver.h"
#include "QFileInfoList"

MyTcpSocket::MyTcpSocket() {
    connect(this, SIGNAL(readyRead())
            , this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected())
            , this, SLOT(clientOffline()));
    m_bUpload = false;
    m_pTimer = new QTimer;
    connect(m_pTimer, SIGNAL(timeout())
            , this, SLOT(sendFileToClient()));
}

void MyTcpSocket::recvMsg() {

//    qDebug() << this->bytesAvailable();
    if (!m_bUpload) {
        uint uiPDULen = 0;

        this->read((char *) &uiPDULen, sizeof(uint));
        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);                                        /*接收的pdu*/
        this->read((char *) pdu + sizeof(uint), uiPDULen);


        switch (pdu->uiMsgType) {
            case ENUM_MSG_TYPE_REGIST_REQUDEST: {
//            qDebug() <<"数据类型" << pdu->uiMsgType;
                char caName[32] = {'\0'};
                char caPwd[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                strncpy(caPwd, pdu->caData + 32, 32);
                qDebug() << "用户名： " << caName << "密码： " << caPwd;
                bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);

                PDU *respdu = mkPDU(0);                                     /*回复的pdu*/

                respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
                if (ret) {
                    strcpy(respdu->caData, REGIST_OK);

                    qDebug() << "注册成功";

                    qDebug() << "创建文件夹";

                    QDir dir;
                    qDebug() << "create dir : "
                             << dir.mkdir(QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1").arg(caName));

                } else {
                    strcpy(respdu->caData, REGIST_FAILED);
                    qDebug() << "注册失败";
                }
                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                pdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_LOGIN_REQUEST: {
                char caName[32] = {'\0'};
                char caPwd[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                strncpy(caPwd, pdu->caData + 32, 32);
                qDebug() << "用户名： " << caName << "密码： " << caPwd;
                bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);

                PDU *respdu = mkPDU(0);                                     /*回复的pdu*/

                respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
                if (ret) {
                    strcpy(respdu->caData, LOGIN_OK);

                    qDebug() << "登录成功";
                    m_strName = caName;

                } else {
                    strcpy(respdu->caData, LOGIN_FAILED);
                    qDebug() << "登录失败";
                }
                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                pdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST: {

                QStringList ret = OpeDB::getInstance().handleAllOnline();
                qDebug() << "返回所有在线用户";

                uint uiMsgLen = ret.size() * 32;
                PDU *respdu = mkPDU(uiPDULen);
                respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
                for (int i = 0; i < ret.size(); ++i) {
                    memcpy((char *) (respdu->caMsg) + i * 32,
                           ret.at(i).toStdString().c_str(),
                           ret.at(i).size());
                }
                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_SERACH_USR_REQUEST: {
                int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_SERACH_USR_RESPOND;
                if (-1 == ret) {
                    strcpy(respdu->caData, SEARCH_USR_NO);
                } else if (1 == ret) {
                    strcpy(respdu->caData, SEARCH_USR_ONLINE);
                } else if (0 == ret) {
                    strcpy(respdu->caData, SEARCH_USR_OFFLINE);
                }

                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: {
                char caPerName[32] = {'\0'};
                char caName[32] = {'\0'};
                strncpy(caPerName, pdu->caData, 32);
                strncpy(caName, pdu->caData + 32, 32);
                int ret = OpeDB::getInstance().handleAddFriend(caPerName, caName);
                PDU *respdu = NULL;

                qDebug() << ret;

                switch (ret) {
                    case -1: {
                        qDebug() << "名称空";
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                        strcpy(respdu->caData, UNKNOW_ERROR);
                        write((char *) respdu, respdu->uiPDULen);
                        free(respdu);
                        respdu = NULL;
                        break;
                    }
                    case 0: {
                        qDebug() << "好友已经存在";
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                        strcpy(respdu->caData, EXISTED_FRIEND);
                        write((char *) respdu, respdu->uiPDULen);
                        free(respdu);
                        respdu = NULL;
                        break;
                    }
                    case 1: {
                        qDebug() << "符合添加好友条件，准备添加好友，返回原pdu";
                        mytcpserver::getInstance().resend(caPerName, pdu);      /*返回原来的pdu*/
                        break;
                    }
                    case 2: {
                        qDebug() << "好友不在线";
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                        strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
                        write((char *) respdu, respdu->uiPDULen);
                        free(respdu);
                        respdu = NULL;
                        break;
                    }
                    case 3: {
                        qDebug() << "用户不存在";
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                        strcpy(respdu->caData, ADD_FRIEND_NOEXIST);
                        write((char *) respdu, respdu->uiPDULen);
                        free(respdu);
                        respdu = NULL;
                        break;
                    }
                }

                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: {
                char caPerName[32] = {'\0'};
                char caName[32] = {'\0'};
                strncpy(caPerName, pdu->caData, 32);
                strncpy(caName, pdu->caData + 32, 32);
                qDebug() << "同意添加好友" << "perName" << caPerName << "name" << caName;
                OpeDB::getInstance().handleAddFriendToSQL(caPerName, caName);
                PDU *respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE: {

                char caPerName[32] = {'\0'};
                char caName[32] = {'\0'};
                strncpy(caPerName, pdu->caData, 32);
                strncpy(caName, pdu->caData + 32, 32);
                qDebug() << caName << "拒绝加" << caPerName << "为好友";
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST: {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);
                uint uiMsgLen = ret.size() * 32;
                PDU *respdu = mkPDU(uiMsgLen);
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
                for (int i = 0; i < ret.size(); ++i) {
                    memcpy((char *) (respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(), ret.at(i).size());
                }
                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST: {
                char strSelfName[32] = {'\0'};
                char strFriendName[32] = {'\0'};
                strncpy(strSelfName, pdu->caData, 32);
                strncpy(strFriendName, pdu->caData + 32, 32);
                OpeDB::getInstance().handleDelFriend(strSelfName, strFriendName);

                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
                strcpy(respdu->caData, DEL_FRIEND_OK);

                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;

                mytcpserver::getInstance().resend(strFriendName, pdu);      /*返回原来的pdu*/

                break;
            }
            case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST: {
                char caPerName[32] = {'\0'};
                memcpy(caPerName, pdu->caData + 32, 32);
                qDebug() << caPerName;
                mytcpserver::getInstance().resend(caPerName, pdu);
                break;
            }
            case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST: {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName);
                QString tmp;
                for (int i = 0; i < onlineFriend.size(); ++i) {
                    tmp = onlineFriend.at(i);
                    mytcpserver::getInstance().resend(tmp.toStdString().c_str(), pdu);
                    qDebug() << "转发到：" << tmp;
                }


                break;
            }
            case ENUM_MSG_TYPE_CREATE_DIR_REQUEST: {
                QDir dir;
                QString strCurPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1").arg(
                        (char *) (pdu->caData));

                qDebug() << strCurPath;

                bool ret = dir.exists(strCurPath);
                PDU *respdu = mkPDU(0);
                if (ret) { /*当前目录存在*/
                    char caNewDir[32] = {'\0'};
                    memcpy(caNewDir, pdu->caData + 32, 32);
                    QString strNewPath = strCurPath + "/" + caNewDir;

                    qDebug() << strNewPath;
                    ret = dir.exists(strNewPath);

                    qDebug() << "-->" << ret;
                    if (ret) { /*创建的文件名已存在*/

//                    PDU *respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                        strcpy(respdu->caData, FILE_NAME_EXIST);

                    } else { /*创建的文件名不存在*/

                        dir.mkdir(strNewPath);
//                    PDU *respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                        strcpy(respdu->caData, CREATE_DIR_OK);

                    }
                } else { /*当前目录不存在*/
                    qDebug() << "当前目录不存在";
//                PDU *respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData, DIR_NO_EXIST);

                }

                write((char *) respdu, respdu->uiPDULen);
                break;
            }
            case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST: {      /*刷新文件返回*/
                qDebug() << "ENUM_MSG_TYPE_FLUSH_FILE_REQUEST";
                char *pCurPath = new char[pdu->uiPDULen];
                memcpy(pCurPath, pdu->caMsg, pdu->uiMsgLen);
                qDebug() << pCurPath;

                QDir dir(QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1").arg(pCurPath));
                QFileInfoList fileInfoList = dir.entryInfoList();

                int iFileCount = fileInfoList.size();
                PDU *respdu = mkPDU(sizeof(FileInfo) * iFileCount);
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

                FileInfo *pFileInfo = NULL;
                QString strFileName;
                for (int i = 0; i < iFileCount; ++i) {

                    pFileInfo = (FileInfo *) (respdu->caMsg) + i;
                    strFileName = fileInfoList[i].fileName();
                    memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());

                    if (fileInfoList[i].isDir()) {
                        pFileInfo->iFileTYpe = 0;
                    } else if (fileInfoList[i].isFile()) {
                        pFileInfo->iFileTYpe = 1;
                    }
                    qDebug() << fileInfoList[i].fileName()
                             << fileInfoList[i].size()
                             << "文件夹：" << fileInfoList[i].isDir()
                             << "常规文件：" << fileInfoList[i].isFile();
                }
                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_DEL_DIR_REQUEST: {       /*删除文件夹请求*/
                char caName[32] = {'\0'};
                strcpy(caName, pdu->caData);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1/%2").arg(pPath).arg(
                        caName);
                qDebug() << strPath;

                QFileInfo fileInfo(strPath);
                bool ret = false;
                if (fileInfo.isDir()) {
                    QDir dir;
                    dir.setPath(strPath);
                    ret = dir.removeRecursively();        /*删除文件夹及其目录下全部文件*/
                } else if (fileInfo.isFile()) {
                    ret = false;
                }

                PDU *respdu = NULL;
                if (ret) {
                    respdu = mkPDU(strlen(DEL_DIR_OK) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                    memcpy(respdu->caData, DEL_DIR_OK, strlen(DEL_DIR_OK));
                } else {
                    respdu = mkPDU(strlen(DEL_DIR_FAILURED) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                    memcpy(respdu->caData, DEL_DIR_FAILURED, strlen(DEL_DIR_FAILURED));
                }
                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_RENAME_FILE_REQUEST: {
                char caOldName[32] = {'\0'};
                char caNewName[32] = {'\0'};

                strncpy(caOldName, pdu->caData, 32);
                strncpy(caNewName, pdu->caData + 32, 32);

                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);

                QString strOldPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1/%2").arg(pPath).arg(
                        caOldName);
                QString strNewPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1/%2").arg(pPath).arg(
                        caNewName);
                qDebug() << strOldPath;
                qDebug() << strNewPath;

                QDir dir;
                bool ret = dir.rename(strOldPath, strNewPath);
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;

                if (ret) {
                    strcpy(respdu->caData, RENAME_FILE_OK);
                } else {
                    strcpy(respdu->caData, RENAME_FILE_FAILURED);
                }

                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_ENTER_DIR_REQUEST: {
                char caEnterName[32] = {'\0'};
                strncpy(caEnterName, pdu->caData, 32);

                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);

                QString strPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1/%2").arg(pPath).arg(
                        caEnterName);

                qDebug() << strPath;

                QFileInfo fileInfo(strPath);
                PDU *respdu = NULL;
                if (fileInfo.isDir()) {
//                QDir dir(QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1").arg(strPath));
                    QDir dir(strPath);
                    QFileInfoList fileInfoList = dir.entryInfoList();

                    int iFileCount = fileInfoList.size();
                    PDU *respdu = mkPDU(sizeof(FileInfo) * iFileCount);
                    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

                    FileInfo *pFileInfo = NULL;
                    QString strFileName;
                    for (int i = 0; i < iFileCount; ++i) {

                        pFileInfo = (FileInfo *) (respdu->caMsg) + i;
                        strFileName = fileInfoList[i].fileName();
                        memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());

                        if (fileInfoList[i].isDir()) {
                            pFileInfo->iFileTYpe = 0;
                        } else if (fileInfoList[i].isFile()) {
                            pFileInfo->iFileTYpe = 1;
                        }
                        qDebug() << fileInfoList[i].fileName()
                                 << fileInfoList[i].size()
                                 << "文件夹：" << fileInfoList[i].isDir()
                                 << "常规文件：" << fileInfoList[i].isFile();
                    }
                    write((char *) respdu, respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;

                } else if (fileInfo.isFile()) {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                    strcpy(respdu->caData, ENTER_DIR_FAILURED);

                    write((char *) respdu, respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                break;
            }
            case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST: {
                qDebug() << "ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST";

                char caFileName[32] = {'\0'};
                qint64 fileSize = 0;
                sscanf(pdu->caData, "%s %11d", caFileName, &fileSize);

                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1/%2").arg(pPath).arg(caFileName);
                qDebug() << strPath;
                free(pPath);
                pPath = NULL;

                m_file.setFileName(strPath);
                /*以只写的方式打开文件，若文件不存在，自动创建文件*/
                if (m_file.open(QIODevice::WriteOnly)) {
                    m_bUpload = true;
                    m_iTotal = fileSize;
                    m_iReceived = 0;
                }

                break;
            }
            case ENUM_MSG_TYPE_DEL_FILE_REQUEST:{
                char caName[32] = {'\0'};
                strcpy(caName, pdu->caData);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1/%2").arg(pPath).arg(
                        caName);
                qDebug() << strPath;

                QFileInfo fileInfo(strPath);
                bool ret = false;
                if (fileInfo.isDir()) {
                    ret = false;
                } else if (fileInfo.isFile()) {
                    QDir dir;
                    ret = dir.remove(strPath);
                }

                PDU *respdu = NULL;
                if (ret) {
                    respdu = mkPDU(strlen(DEL_FILE_OK) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                    memcpy(respdu->caData, DEL_FILE_OK, strlen(DEL_FILE_OK));
                } else {
                    respdu = mkPDU(strlen(DEL_FILE_FAILURED) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                    memcpy(respdu->caData, DEL_FILE_FAILURED, strlen(DEL_FILE_FAILURED));
                }
                write((char *) respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:{
                qDebug() << "ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST";

                char caFileName[32] = {'\0'};
                strcpy(caFileName,pdu->caData);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1/%2").arg(pPath).arg(caFileName);
                qDebug() << strPath;
                free(pPath);
                pPath = NULL;

                QFileInfo fileInfo(strPath);
                qint64 fileSize = fileInfo.size();

                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
                sprintf(respdu->caData,"%s %11d",caFileName,fileSize);
                qDebug() << respdu->caData;
                write((char *)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;

                m_file.setFileName(strPath);
                qDebug() <<"m_file" << m_file;
                m_file.open(QIODevice::ReadOnly);
                m_pTimer->start(1000);
                qDebug() << "开启定时器";
                break;
            }
            case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
                qDebug() << "ENUM_MSG_TYPE_SHARE_FILE_REQUEST";
                char caSendName[32] = {'\0'};
                int num = 0;
                sscanf(pdu->caData,"%s %d",caSendName,&num);
                int size = num*32;
                PDU *respdu = mkPDU(pdu->uiMsgLen-size);
                respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;

                strcpy(respdu->caData,caSendName);
                memcpy(respdu->caMsg,(char *)(pdu->caMsg)+size,pdu->uiPDULen-size);

                qDebug() << "caSendName" << respdu->caData;
                qDebug() << "caMsg" << respdu->caMsg;

                char caRecvName[32] = {'\0'};
                for (int i = 0; i < num; ++i) {
                    memcpy(caRecvName,(char *)(pdu->caMsg)+i*32,32);
                    qDebug() << "转发到：" << caRecvName;
                    mytcpserver::getInstance().resend(caRecvName,respdu);
                }
                free(respdu);
                respdu = NULL;

                qDebug() << "/*发送成功反馈*/";
                respdu = mkPDU(0);                                 /*发送成功反馈*/
                respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
                strcpy(respdu->caData,"share file ok");

                qDebug() << respdu->caData;

                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;

                break;
            }
            case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND:{
                qDebug() << "ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND";
                QString strRecvPath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1").arg(pdu->caData);
                QString strShareFilePath = QString("/home/fumoumou/Desktop/NetDisk/TcpServer/UsrFile/%1").arg((char *)(pdu->caMsg));

                int index = strShareFilePath.lastIndexOf('/');
                QString strFileName = strShareFilePath.right(strShareFilePath.size()-index-1);
                strRecvPath = strRecvPath + '/' + strFileName;

                QFileInfo fileInfo(strShareFilePath);

                qDebug() << "接收目录" << strRecvPath;
                qDebug() << "分享目录" << strShareFilePath;

                if (fileInfo.isFile())
                {
                    QFile::copy(strShareFilePath,strRecvPath);
                    qDebug() << "分享目录完成";
                } else if (fileInfo.isDir())
                {
                    copyDir(strShareFilePath,strRecvPath);
                }
                break;
            }
            default: {
                break;
            }

        }
        free(pdu);
        pdu = NULL;
    } else
    {
        PDU *respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;

        /*接受文件的二进制数据*/
        QByteArray buff = readAll();
        m_file.write(buff);
        m_iReceived +=buff.size();
        if (m_iTotal == m_iReceived)
        {
            m_file.close();
            m_bUpload = false;

            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char *)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        } else if (m_iTotal < m_iReceived)
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            write((char *)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }

    }

}

void MyTcpSocket::clientOffline() {
    qDebug() << "客户端下线，用户名：" << m_strName;
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);

}


QString MyTcpSocket::getName() {
    return m_strName;
}

void MyTcpSocket::sendFileToClient() {
    m_pTimer->stop();
    qDebug() << "关闭定时器";
    qDebug() << "发送文件到客户端";
    char *pData = new char[4096];
    qint64 ret = 0;
    while (true)
    {
        ret = m_file.read(pData,4096);
        if (ret > 0 && ret <= 4096)
        {
            write(pData,ret);
        } else if (0 == ret)
        {
            m_file.close();
            break;
        } else if (ret < 0)
        {
            qDebug() << "发送文件给客户端过程中失败";
            m_file.close();
            break;
        }
    }
    qDebug() << "发送完成";
//    free(pData);
    delete[] pData;
    qDebug() << "释放pData";
    pData = NULL;
}

void MyTcpSocket::copyDir(QString strSrcDir ,QString strDestDir) {
    QDir dir;
    dir.mkdir(strDestDir);
    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();
    QString srcTmp;
    QString destTmp;
    for (int i = 0; i < fileInfoList.size(); ++i) {
        qDebug() << "filename:" << fileInfoList[i].fileName();
        if (fileInfoList[i].isFile())
        {
            fileInfoList[i].fileName();
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strDestDir + '/'+fileInfoList[i].fileName();
            QFile::copy(srcTmp,destTmp);
        } else if (fileInfoList[i].isDir())
        {
            if (fileInfoList[i].fileName() == QString("..") || fileInfoList[i].fileName() == QString("."))
            {
                continue;
            }
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strDestDir + '/'+fileInfoList[i].fileName();
            copyDir(srcTmp,destTmp);
        }
    }

}

