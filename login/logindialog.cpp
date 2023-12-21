#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->usrLineEdit->setFocus();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}


// 登录验证逻辑
void LoginDialog::showWeiChatWindow()
{
    QFile file("login/userinfo.xml");
    mydoc.setContent(&file);
    QDomElement root = mydoc.documentElement();
    if (root.hasChildNodes())
    {
        QDomNodeList userList = root.childNodes();
        bool flag = false;
        int len = userList.count();
        for (int i = 0; i < len; i++) // 遍历xml文件
        {
            QDomNode user = userList.at(i);
            QDomNodeList record = user.childNodes();
            QString username = record.at(0).toElement().text();
            QString password = record.at(1).toElement().text();
            if (username == ui->usrLineEdit->text()) // 用户名存在
            {
                flag = true;
                if (password != ui->pwdLineEdit->text()) // 密码错误
                {
                    QMessageBox::warning(0, QObject::tr("error"), "Wrong Password!");
                    ui->pwdLineEdit->clear();
                    ui->pwdLineEdit->setFocus(); // 清空密码，指针回到密码，符合操作习惯
                    return;
                }
            }
        }
        if (!flag) // 用户名不存在
        {
            QMessageBox::warning(0, QObject::tr("error"), "User not exist!");
            ui->usrLineEdit->clear();
            ui->pwdLineEdit->clear();
            ui->usrLineEdit->setFocus(); // 全清空，指针回到用户名
            return;
        }
        // 用户名和密码都正确
        weiChatWindow = new MainWindow(0);
        weiChatWindow->setWindowTitle(ui->usrLineEdit->text());
        weiChatWindow->show();
    }
}
