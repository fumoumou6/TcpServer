#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDialog>
#include <QFile>
#include "QMessageBox"
#include "QByteArray"
#include "QDebug"
#include "QHostAddress"
#include "../func/OpeDB.h"
namespace Ui {
class tcpserver;
}

class tcpserver : public QDialog
{
    Q_OBJECT

public:
    explicit tcpserver(QWidget *parent = nullptr);
    ~tcpserver();
    void loadConfig();

private:
    Ui::tcpserver *ui;
    QString m_strIP;
    quint16 m_usPort;
};

#endif // TCPSERVER_H
