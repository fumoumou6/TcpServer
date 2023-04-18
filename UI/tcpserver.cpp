#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "../func/mytcpserver.h"
tcpserver::tcpserver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tcpserver)
{
    ui->setupUi(this);
    loadConfig();
    mytcpserver::getInstance().listen(QHostAddress(m_strIP),m_usPort);


}
void tcpserver::loadConfig()
{
    QString runPath = QCoreApplication::applicationDirPath() + "/../resource/server.config";
//    qDebug() <<runPath;
    QFile file(runPath);
    if(file.open(QIODevice::ReadOnly)){
        QByteArray bytedata = file.readAll();
        QString strData = bytedata.toStdString().c_str();
//        qDebug() << strData;
        file.close();

        strData.replace("\n"," ");
//        qDebug() << strData;

        QStringList strlist = strData.split(" ");
//        for (int i = 0; i < strlist.size(); ++i) {
//            qDebug() << "--->" << strlist[i];
//        }
        m_strIP = strlist.at(0);
        m_usPort = strlist.at(1).toUShort();
        qDebug() << "ip:" << m_strIP << " port:" << m_usPort;



    } else{

        QMessageBox::critical(this,"open config","open config fail");
    }
}


tcpserver::~tcpserver()
{
    delete ui;
}
