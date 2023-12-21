#include "filesrvdlg.h"
#include "ui_filesrvdlg.h"

FileSrvDlg::FileSrvDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSrvDlg)
{
    ui->setupUi(this);
    myTcpSrv = new QTcpServer(this);
    mySrvPort = 5555;
    connect(myTcpSrv, SIGNAL(newConnection()), this, SLOT(sndChatMsg()));
    myTcpSrv->close();

    // 初始化变量
    myTotalBytes = 0;
    mySendBytes = 0;
    myBytesTobeSend = 0;
    myPayloadSize = 64 * 1024;

    // 初始化组件状态
    ui->sendProgressBar->reset();
    ui->openFilePushButton->setEnabled(true);
    ui->sendFilePushButton->setEnabled(false);
}

void FileSrvDlg::sndChatMsg()
{
    ui->sendFilePushButton->setEnabled(false);
    mySrvSocket = myTcpSrv->nextPendingConnection();
    connect(mySrvSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(refreshProgress(qint64)));

    // 获取待发送文件的路径和大小
    myLocalPathFile = new QFile(myPathFile);
    myLocalPathFile->open((QFile::ReadOnly));
    myTotalBytes = myLocalPathFile->size();

    // 将文件内容读取到myOutputBlock中
    QDataStream sendOut(&myOutputBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_11);
    mytime.start();

    QString currentFile = myPathFile.right(myPathFile.size() - myPathFile.lastIndexOf('/') - 1);

    // qDebug() << currentFile;

    sendOut << qint64(0) << qint64(0) << currentFile;
    myTotalBytes += myOutputBlock.size();

    // 从文件头开始读写操作
    sendOut.device()->seek(0);
    sendOut << myTotalBytes << qint64((myOutputBlock.size() - sizeof(qint64) * 2));

    /*
    qDebug() << "myTotalBytes: " << myTotalBytes;
    qDebug() << "mySendBytes: " << mySendBytes;
    qDebug() << "myBytesTobeSend: " << myBytesTobeSend;
    */

    myBytesTobeSend = myTotalBytes - mySrvSocket->write(myOutputBlock);
    myOutputBlock.resize(0); // 清空发送缓存
}

void FileSrvDlg::refreshProgress(qint64 bynum)
{
    qApp->processEvents();
    mySendBytes += (int)bynum;
    // 文件发送处理
    if (myBytesTobeSend > 0)
    {
        myOutputBlock = myLocalPathFile->read(qMin(myBytesTobeSend, myPayloadSize));
        myBytesTobeSend -= (int)mySrvSocket->write(myOutputBlock);
        myOutputBlock.resize(0);
    }
    else {myLocalPathFile->close();}

    // ui部分设置
    ui->sendProgressBar->setMaximum(myTotalBytes);
    ui->sendProgressBar->setValue(mySendBytes);
    ui->sfileSizeLineEdit->setText(tr("%1").arg(myTotalBytes / (1024 * 1024)) + "MB");
    ui->sendSizeLineEdit->setText(tr("%1").arg(mySendBytes / (1024 * 1024)) + "MB");
    if (mySendBytes == myTotalBytes) // 发送完成弹窗
    {
        myLocalPathFile->close();
        myTcpSrv->close();
        QMessageBox::information(0, QObject::tr("Finished"), "File transfer completed.");
    }
}

void FileSrvDlg::on_openFilePushButton_clicked()
{
    myPathFile = QFileDialog::getOpenFileName(this);
    if (!myPathFile.isEmpty())
    {
        myFileName = myPathFile.right(myPathFile.size() - myPathFile.lastIndexOf('/') - 1);
        ui->sfileNameLineEdit->setText(tr("%1").arg(myFileName));
        ui->sendFilePushButton->setEnabled(true);
        ui->openFilePushButton->setEnabled(false);
    }
}

void FileSrvDlg::on_sendFilePushButton_clicked()
{
    if (!myTcpSrv->listen(QHostAddress::Any, mySrvPort)) // 监听
    {
        QMessageBox::warning(0, QObject::tr("error"), "Failed to open the TCP port, Please check the network connection.");
        close();
        return;
    }
    emit sendFileName(myFileName);
}

void FileSrvDlg::on_srvClosePushButton_clicked()
{
    ui->openFilePushButton->setEnabled(true);
    ui->sendFilePushButton->setEnabled(false);
    if (myTcpSrv->isListening())
    {
        myTcpSrv->close();
        myLocalPathFile->close();
        mySrvSocket->abort();
    }
    close();
}
FileSrvDlg::~FileSrvDlg()
{
    delete ui;
}
