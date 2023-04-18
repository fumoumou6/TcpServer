/********************************************************************************
** Form generated from reading UI file 'tcpserver.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPSERVER_H
#define UI_TCPSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_tcpserver
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *tcpserver)
    {
        if (tcpserver->objectName().isEmpty())
            tcpserver->setObjectName(QString::fromUtf8("tcpserver"));
        tcpserver->resize(400, 300);
        buttonBox = new QDialogButtonBox(tcpserver);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(tcpserver);
        QObject::connect(buttonBox, SIGNAL(accepted()), tcpserver, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), tcpserver, SLOT(reject()));

        QMetaObject::connectSlotsByName(tcpserver);
    } // setupUi

    void retranslateUi(QDialog *tcpserver)
    {
        tcpserver->setWindowTitle(QCoreApplication::translate("tcpserver", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class tcpserver: public Ui_tcpserver {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPSERVER_H
