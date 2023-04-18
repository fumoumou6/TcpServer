//
// Created by fumoumou on 23-3-23.
//

#ifndef TCPSERVER_OPEDB_H
#define TCPSERVER_OPEDB_H

#include "QObject"
#include "QSqlDatabase"
#include "QSqlQuery"
#include "QMessageBox"
#include "QDebug"
#include "QStringList"
class OpeDB : public QObject{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = 0);
    static OpeDB& getInstance();
    void init();
    ~OpeDB();
    bool handleRegist(const char *name,const char *pwd);
    bool handleLogin(const char *name,const char *pwd);
    bool handleOffline(const char *name);
    QStringList handleAllOnline();
    int handleSearchUsr(const char *name);
    int handleAddFriend(const char *pername,const char *name);
    void handleAddFriendToSQL(const char *pername,const char *name);
    QStringList handleFlushFriend(const char *name);
    bool handleDelFriend(const char *name,const char *friendname);
signals:

public slots:
private:
    QSqlDatabase m_db;


};


#endif //TCPSERVER_OPEDB_H
