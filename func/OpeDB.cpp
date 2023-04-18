//
// Created by fumoumou on 23-3-23.
//

#include "OpeDB.h"
OpeDB::OpeDB(QObject *parent) : QObject(parent){
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance() {
    static OpeDB instance;
    return instance;
}

void OpeDB::init() {
    m_db.setHostName("localhost");
    m_db.setDatabaseName("../database/cloud.db");
    if (m_db.open()){
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while (query.next()){
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }


    } else{
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

OpeDB::~OpeDB() {
    m_db.close();

    qDebug() << "m_db close";

}

bool OpeDB::handleRegist(const char *name, const char *pwd) {
    if (NULL == name || NULL == pwd){
        qDebug() << "帐号密码空字符";
        return false;
    }
    QString data = QString("insert into usrInfo(name, pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    qDebug() << data;
    QSqlQuery query;
    return query.exec(data);

}

bool OpeDB::handleLogin(const char *name, const char *pwd) {
    if (NULL == name || NULL == pwd){
        qDebug() << "帐号密码空字符";
        return false;
    }
    QString data = QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data); /*执行*/
    if (query.next()) {             /*获取匹配项*/

        /*设置登录状态*/
        data = QString("update usrInfo set online=1 where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
        qDebug() << data;
        QSqlQuery query;
        query.exec(data); /*执行*/
        return true;
    } else{
        return query.next();
    }


}

bool OpeDB::handleOffline(const char *name) {
    if (NULL == name ){
        qDebug() << "帐号为空";
        return false;
    }
    QString data = QString("update usrInfo set online=0 where name=\'%1\'").arg(name);
    qDebug() << data;
    QSqlQuery query;
    return query.exec(data);

}

QStringList OpeDB::handleAllOnline() {
    QString data = QString("select name from usrInfo where online=1");
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while (query.next()){
        result.append(query.value(0).toString());

    }
    return result;
}

int OpeDB::handleSearchUsr(const char *name) {
    if (NULL == name){
        return -1;                   /*不存在用户*/
    }
    QString data = QString("select online from usrInfo where name=\'%1\'").arg(name);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if (query.next()){
        int ret = query.value(0).toInt();
        if (1 == ret){
            return 1;                  /*在线*/
        } else if(0 == ret){
            return 0;                   /*不在线*/
        }
    } else{
        return -1;                       /*不存在用户*/
    }
}
/*********************************/
/*三个输出值
 * 名称空 -1
 * 已经是好友 0
 * 不是好友但对方在线 1
 * 不是好友对方不在线 2
 * 不存在对方 3*/
/*********************************/
int OpeDB::handleAddFriend(const char *pername, const char *name) {
    if (NULL == pername || NULL == name){   /*错误信息*/
        return -1;
    }
    QString data = QString("select * from friend where ("
                           "id=(select id from usrInfo where name=\'%1\' ) and friendId = (select id from usrInfo where name=\'%2\'))"
                           " or (id=(select id from usrInfo where name=\'%3\') and friendId = (select id from usrInfo where name=\'%4\'))").arg(pername).arg(name).arg(name).arg(pername);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);

    if (query.next()){
        return 0;         /*已经是好友*/
    } else{               /*不是好友判断*/
         data = QString("select online from usrInfo where name=\'%1\'").arg(pername);
        qDebug() << data;

        query.exec(data);
        if (query.next()){
            int ret = query.value(0).toInt();
            if (1 == ret){
                return 1;                  /*在线*/
            } else if(0 == ret){
                return 2;                   /*不在线*/
            }
        } else{
            return 3;                       /*不存在用户*/
        }
    }
}

void OpeDB::handleAddFriendToSQL(const char *pername, const char *name) {
    if (NULL == pername || NULL == name){   /*错误信息*/
        return;
    }
    QString data = QString("insert into friend (id,friendId) values("
                           "(select id from usrInfo where name=\'%1\'),(select id from usrInfo where name=\'%2\'))").arg(pername).arg(name);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);

}

QStringList OpeDB::handleFlushFriend(const char *name) {
    QStringList strFriendList;
    strFriendList.clear();
    if (NULL == name){
        return strFriendList;
    }
    /*作为id*/
    QString data = QString("select name from usrInfo where online=1 and id in (select id from friend where friendId=(select id from usrInfo where name=\'%1\'))").arg(name);

    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    while (query.next()){
        strFriendList.append(query.value(0).toString());
        qDebug() << query.value(0).toString();

    }

    query.clear();          /*清理query*/

    /*作为friendid*/
    data = QString("select name from usrInfo where online=1 and id in (select friendId from friend where id=(select id from usrInfo where name=\'%1\'))").arg(name);

    qDebug() << data;

    query.exec(data);
    while (query.next()){
        strFriendList.append(query.value(0).toString());
        qDebug() << query.value(0).toString();
    }
    return strFriendList;
}

bool OpeDB::handleDelFriend(const char *name, const char *friendname) {

    if (NULL == name || NULL == friendname){
        return false;
    }
    QString data = QString("delete from friend where id=(select id from usrInfo where name=\'%1\') and friendId=(select id from usrInfo where name=\'%2\')").arg(name).arg(friendname);

    qDebug() << data;

    QSqlQuery query;
    query.exec(data);

    data = QString("delete from friend where id=(select id from usrInfo where name=\'%1\') and friendId=(select id from usrInfo where name=\'%2\')").arg(friendname).arg(name);
    qDebug() << data;
    query.exec(data);
    return true;
}

