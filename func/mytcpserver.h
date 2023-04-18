//
// Created by fumoumou on 23-3-13.
//

#ifndef TCPSERVER_MYTCPSERVER_H
#define TCPSERVER_MYTCPSERVER_H

#include "QTcpServer"
#include "QList"
#include "mytcpsocket.h"
#include "portocol.h"

class mytcpserver : public QTcpServer{
    Q_OBJECT
public:
    mytcpserver();
    static mytcpserver &getInstance();
    void incomingConnection(qintptr socketDEscriptor);

    void resend(const char *pername,PDU *pdu);

public slots:
    void delteSocket(MyTcpSocket *mysocket);

private:
    QList<MyTcpSocket*> m_tcpSocketList;
};


#endif //TCPSERVER_MYTCPSERVER_H
