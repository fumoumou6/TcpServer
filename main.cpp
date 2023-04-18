#include <QApplication>
#include <QPushButton>
#include "UI/tcpserver.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    OpeDB::getInstance().init();
    tcpserver w;
    w.show();
    return a.exec();
}
