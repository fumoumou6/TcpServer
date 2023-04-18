//
// Created by fumoumou on 23-3-20.
//

#ifndef TCPSERVER_MYTCPSOCKET_H
#define TCPSERVER_MYTCPSOCKET_H

#include "QTcpSocket"
#include "OpeDB.h"

class MyTcpSocket : public QTcpSocket{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();

signals:
    void offline(MyTcpSocket *mysocket);


public slots:
    void recvMsg();
    void clientOffline();

private:
    QString m_strName;
};

#endif //TCPSERVER_MYTCPSOCKET_H
