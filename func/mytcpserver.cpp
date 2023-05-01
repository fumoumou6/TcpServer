//
// Created by fumoumou on 23-3-13.
//

#include "mytcpserver.h"
mytcpserver::mytcpserver() {

}

mytcpserver &mytcpserver::getInstance() {
    qDebug() << "mytcpserver::getInstance";
    static mytcpserver instance;
    return instance;
}

void mytcpserver::incomingConnection(qintptr socketDEscriptor) {
    qDebug("new client connected\r\n");
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDEscriptor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)),
            this, SLOT(delteSocket(MyTcpSocket*)));
}

void mytcpserver::delteSocket(MyTcpSocket *mysocket) {

    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for (; iter != m_tcpSocketList.end(); iter++) {
        if (mysocket == *iter){
//            delete *iter;            /*不能直接delete  要用延后delete*/
            (*iter)->deleteLater();
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }
    for (int i = 0; i < m_tcpSocketList.size(); i++) {
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}

void mytcpserver::resend(const char *pername, PDU *pdu) {
    if (NULL == pername || NULL == pdu){
        qDebug() << "转发失败";
        return;
    }
    QString strName = pername;
    for (int i = 0; i < m_tcpSocketList.size(); i++) {
        qDebug() << m_tcpSocketList.at(i)->getName();
        if (strName == m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write((char *)pdu,pdu->uiPDULen);
            break;
        }
    }
}
