#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QDateTime>
#include <QUdpSocket>
#include <QFile>
#include <QFileDialog>
#include "qdom.h"
#include "filesrv/filesrvdlg.h"
#include "filecnt/filecntdlg.h"

enum ChatMsgType { ChatMsg, OnLine, OffLine, SfileName, RefFile}; // 5种UDP消息类型：发送文件，在线，离线，文件名

class FileSrvDlg;
class FileCntDlg;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initMainWindow();
    void onLine(QString name, QString time);
    void offLine(QString name, QString time);
    void sendChatMsg(ChatMsgType msgType, QString rmtName = "");
    int findAvailablePort(int startingPort);
    QString getLocalHostIp();
    QString getLocalChatMsg();

    void recvFileName(QString name, QString hostip, QString rmtname, QString filename);

protected:
    void closeEvent(QCloseEvent *event) {sendChatMsg(OffLine);}

private slots:
    void on_sendPushButton_clicked(); // 点击发送消息
    void recvAndProcessChatMsg();
    void on_searchPushButton_clicked();

    void getSfileName(QString);
    void on_transPushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString myname = "";
    QUdpSocket *myUdpSocket;
    qint16 myUdpPort;
    QDomDocument myDoc;

    QString myFileName;
    FileSrvDlg *myfsrv;
};

#endif // MAINWINDOW_H
