#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initMainWindow();
}

void MainWindow::initMainWindow() // 初始化UDP套接字，端口号，绑定槽函数
{
    myUdpSocket = new QUdpSocket(this);
    myUdpPort = findAvailablePort(23232); // 查找可用端口
    myUdpSocket->bind(myUdpPort, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(myUdpSocket, SIGNAL(readyRead()), this, SLOT(recvAndProcessChatMsg()));

    myfsrv = new FileSrvDlg(this);
    connect(myfsrv, SIGNAL(sendFileName(QString)), this, SLOT(getSfileName(QString)));
}

int MainWindow::findAvailablePort(int startingPort)
{
    for (int port = startingPort; port < 65535; port++) // 从指定端口开始逐个尝试
    {
        QUdpSocket socket;
        if (socket.bind(port, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint))
        {
            socket.close();
            return port; // 找到可用端口
        }
    }
    return -1; // 未找到可用端口
}

void MainWindow::sendChatMsg(ChatMsgType msgType, QString rmtName) // 发送UDP广播消息
{
    QByteArray arr;
    QDataStream write(&arr, QIODevice::WriteOnly);
    QString localHostIp = getLocalHostIp();
    QString localHostMsg = getLocalChatMsg();
    write << msgType << myname;
    switch(msgType) // 根据UDP消息类型判断下一步行为
    {
        case ChatMsg: {write << localHostIp << localHostMsg; break;}
        case OnLine: {write << localHostIp; break;}
        case OffLine: break;
        case SfileName: {write << localHostIp << rmtName << myFileName; break;}
        case RefFile: {write << localHostIp << rmtName; break;}
    }
    myUdpSocket->writeDatagram(arr, arr.length(), QHostAddress::Broadcast, myUdpPort); //处理完消息后用writeDatagram()函数将其广播
}

void MainWindow::recvAndProcessChatMsg() // 接收UDP广播消息
{
    if(myUdpSocket->hasPendingDatagrams()) // 如有数据报待处理
    {
        QByteArray arr;
        arr.resize(myUdpSocket->pendingDatagramSize()); // 为待读取的数据分配缓冲区
        myUdpSocket->readDatagram(arr.data(), arr.size()); // 读数据
        QDataStream read(&arr, QIODevice::ReadOnly);
        int msgType;
        read >> msgType;
        QString name, hostip, chatmsg, rname, fname;
        QString currenttime = QDateTime::currentDateTime().toString(("yyyy-MM-dd hh:mm:ss"));
        switch (msgType)
        {
            case ChatMsg:
            {
                // 类型是消息，则读取相关信息，并将消息显示
                read >> name >> hostip >> chatmsg;
                ui->chatTextBrowser->setTextColor(Qt::darkGreen);
                ui->chatTextBrowser->setCurrentFont(QFont("Times New Roman", 14));
                ui->chatTextBrowser->append(name + " " + currenttime);
                ui->chatTextBrowser->append(chatmsg);
                break;
            }
            case OnLine:
            {
                // 类型是在线，则广播昵称和时间
                read >> name >> hostip;
                onLine(name, currenttime);
                break;
            }
            case OffLine:
            {
                // 类型是离线，则广播昵称和时间
                read >> name;
                offLine(name, currenttime);
                break;
            }
            case SfileName:
            {
                // 类型是文件，则处理文件发送与接收
                read >> name >> hostip >> rname >> fname;
                recvFileName(name, hostip, rname, fname);
                break;
            }
            case RefFile:
            {
                // 类型是文件拒收则检索并处理
                read >> name >> hostip >> rname;
                if (myname == rname) myfsrv->cntRefused();
            }
        }
    }
}

void MainWindow::on_searchPushButton_clicked()
{
    myname = this->windowTitle(); // 之前设置过的窗口名是用户昵称
    ui->userLable->setText(myname);
    sendChatMsg(OnLine);
}

void MainWindow::onLine(QString name, QString time) // 上线处理函数
{
    if (ui->userListTableWidget->findItems(name, Qt::MatchContains).isEmpty()) // 用户不在线就发送广播
    {
        int rowCount = ui->userListTableWidget->rowCount();
        ui->userListTableWidget->setColumnCount(1);
        ui->userListTableWidget->insertRow(rowCount);
        //ui->userListTableWidget->item(rowCount, 0)->setFlags(ui->userListTableWidget->item(rowCount, 0)->flags() & ~Qt::ItemIsEditable);
        // qDebug() << "New row count: " << ui->userListTableWidget->rowCount();

        ui->userListTableWidget->setItem(rowCount, 0, new QTableWidgetItem(name)); // 设置新的表格项

        ui->chatTextBrowser->setTextColor(Qt::gray);
        ui->chatTextBrowser->setCurrentFont(QFont("Times new Roman", 12));
        ui->chatTextBrowser->append(name + " is online at " + time + ".");
        // sendChatMsg(OnLine, name);
        ui->userListTableWidget->viewport()->update(); // 刷新用户表格的视图
    }
}

void MainWindow::offLine(QString name, QString time)// 用户离线处理函数
{
    int row = ui->userListTableWidget->findItems(name, Qt::MatchExactly).first()->row();
    ui->userListTableWidget->removeRow(row);
    ui->chatTextBrowser->setTextColor(Qt::gray);
    ui->chatTextBrowser->setCurrentFont(QFont("Times new Roman", 12));
    ui->chatTextBrowser->append(name + " is offline at " + time + ".");
}

QString MainWindow::getLocalHostIp() // 获取本机IP地址
{
    QList<QHostAddress> addrlist = QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, addrlist)
    {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol) return addr.toString();
    }
    return 0;
}

QString MainWindow::getLocalChatMsg() // 获取聊天信息
{
    QString chatmsg = ui->ChatTextEdit->toHtml();
    ui->ChatTextEdit->clear();
    ui->ChatTextEdit->setFocus();
    return chatmsg;
}

void MainWindow::on_sendPushButton_clicked()
{
     sendChatMsg(ChatMsg);
}


void MainWindow::getSfileName(QString fname)
{
    myFileName = fname;
    int row = ui->userListTableWidget->currentRow();
    QString rmtName = ui->userListTableWidget->item(row, 0)->text();
    sendChatMsg(SfileName, rmtName);
}

void MainWindow::on_transPushButton_clicked()
{
    if (ui->userListTableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0, tr("Select a friend."), tr("Please select the file recipient first."), QMessageBox::Ok);
        return;
    }
    myfsrv->show();
}

void MainWindow::recvFileName(QString name, QString hostip, QString rmtname, QString filename)
{
    if (myname == rmtname)
    {
        int result = QMessageBox::information(this, "Receive a document.", QString("Your friend %1 sent you a file: \r\n%2, accept or not?").arg(name).arg(filename), QMessageBox::Yes, QMessageBox::No);
        if (result == QMessageBox::Yes)
        {
            QString fname = QFileDialog::getSaveFileName(0, "save", filename);
            if (!fname.isEmpty())
            {
                FileCntDlg *fcnt = new FileCntDlg(this);
                fcnt->getLocalPath(fname);
                fcnt->getSrvAddr(QHostAddress(hostip));
                fcnt->show();
            }
            else sendChatMsg(RefFile, name);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
