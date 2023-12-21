/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QLabel *label;
    QLineEdit *usrLineEdit;
    QLineEdit *pwdLineEdit;
    QPushButton *loginPushButton;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QStringLiteral("LoginDialog"));
        LoginDialog->resize(339, 467);
        label = new QLabel(LoginDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 0, 360, 469));
        label->setStyleSheet(QStringLiteral(""));
        label->setFrameShape(QFrame::NoFrame);
        label->setFrameShadow(QFrame::Sunken);
        label->setPixmap(QPixmap(QString::fromUtf8("loginpic.jpg")));
        usrLineEdit = new QLineEdit(LoginDialog);
        usrLineEdit->setObjectName(QStringLiteral("usrLineEdit"));
        usrLineEdit->setGeometry(QRect(112, 242, 120, 20));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(10);
        usrLineEdit->setFont(font);
        usrLineEdit->setAlignment(Qt::AlignCenter);
        pwdLineEdit = new QLineEdit(LoginDialog);
        pwdLineEdit->setObjectName(QStringLiteral("pwdLineEdit"));
        pwdLineEdit->setGeometry(QRect(112, 284, 120, 20));
        pwdLineEdit->setFont(font);
        pwdLineEdit->setEchoMode(QLineEdit::Password);
        pwdLineEdit->setAlignment(Qt::AlignCenter);
        loginPushButton = new QPushButton(LoginDialog);
        loginPushButton->setObjectName(QStringLiteral("loginPushButton"));
        loginPushButton->setGeometry(QRect(63, 328, 224, 51));
        loginPushButton->setFlat(true);

        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "Dialog", nullptr));
        label->setText(QString());
        loginPushButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
