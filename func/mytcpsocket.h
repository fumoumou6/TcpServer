//
// Created by fumoumou on 23-3-20.
//

#ifndef TCPSERVER_MYTCPSOCKET_H
#define TCPSERVER_MYTCPSOCKET_H

#include "QTcpSocket"
#include "OpeDB.h"
#include "QDir"
#include "QFile"
#include "QTimer"
class MyTcpSocket : public QTcpSocket{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
    void copyDir(QString strSrcDir ,QString strDestDir);

signals:
    void offline(MyTcpSocket *mysocket);


public slots:
    void recvMsg();
    void clientOffline();
    void sendFileToClient();

private:
    QString m_strName;
    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iReceived;
    bool m_bUpload;

    QTimer *m_pTimer;
};

#endif //TCPSERVER_MYTCPSOCKET_H
