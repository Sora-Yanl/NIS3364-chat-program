#ifndef FILECNTDLG_H
#define FILECNTDLG_H

#include <QDialog>
#include <QFile>
#include <QTime>
#include <QTcpSocket>
#include <QHostAddress>

class QTcpSocket;

namespace Ui {
class FileCntDlg;
}

class FileCntDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FileCntDlg(QWidget *parent = 0);
    ~FileCntDlg();
    void getSrvAddr(QHostAddress saddr) {mySrvAddr = saddr; createConnectToSrv();} // 获取服务器IP
    void getLocalPath(QString lpath) {myLocalPathFile = new QFile(lpath);}

protected:
    void closeEvent(QCloseEvent *) {on_cntClosePushButton_clicked();}

private slots:
    void createConnectToSrv(); // 创造与服务器的连接
    void readChatMsg();
    void on_cntClosePushButton_clicked();

private:
    Ui::FileCntDlg *ui;
    QTcpSocket *myCntSocket; // 客户端套接字指针
    QHostAddress mySrvAddr;
    qint16 mySrvPort;

    // 文件相关信息
    QFile *myLocalPathFile;
    QString myFileName;
    qint64 myFileNameSize;

    // 文件传输相关信息
    qint64 myTotalBytes;
    qint64 myRcvedBytes;
    qint16 myBlockSize;
    QByteArray myInputBlock;
    QTime mytime;
};

#endif // FILECNTDLG_H
