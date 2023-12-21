#include "filecntdlg.h"
#include "ui_filecntdlg.h"

FileCntDlg::FileCntDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileCntDlg)
{
    // 初始化
    ui->setupUi(this);
    myCntSocket = new QTcpSocket(this);
    mySrvPort = 5555;
    connect(myCntSocket, SIGNAL(readyRead()), this, SLOT(readChatMsg()));
    myFileNameSize = 0;
    myTotalBytes = 0;
    myRcvedBytes = 0;
}

void FileCntDlg::createConnectToSrv()
{
    myBlockSize = 0;
    myCntSocket->abort();
    myCntSocket->connectToHost(mySrvAddr, mySrvPort);
    mytime.start();
}

void FileCntDlg::readChatMsg()
{
    QDataStream in(myCntSocket);
    in.setVersion(QDataStream::Qt_5_11); // 设置版本信息
    float usedTime = mytime.elapsed();
    if (myRcvedBytes <= sizeof(qint64) * 2) // 载入文件名和大小
    {
        if ((myCntSocket->bytesAvailable() >= sizeof(qint64) * 2) && (myFileNameSize == 0))
        {
            in >> myTotalBytes >> myFileNameSize;
            myRcvedBytes += sizeof(qint64) * 2;
        }
        if ((myCntSocket->bytesAvailable() >= myFileNameSize) && (myFileNameSize != 0))
        {
            in >> myFileName;
            myRcvedBytes += myFileNameSize;
            myLocalPathFile->open(QFile::WriteOnly);
            ui->rfileNameLineEdit->setText(myFileName);
        }
        else return;
    }
    if (myRcvedBytes < myTotalBytes) // 接收未完成
    {
        myRcvedBytes += myCntSocket->bytesAvailable();
        myInputBlock = myCntSocket->readAll();
        myLocalPathFile->write(myInputBlock);
        myInputBlock.resize(0);
    }

    /*
    qDebug() << "myRcvedBytes: " << myRcvedBytes;
    qDebug() << "myTotalBytes: " << myTotalBytes;
    qDebug() << "myFileNameSize: " << myFileNameSize;
    qDebug() << "myCntSocket->bytesAvailable(): " << myCntSocket->bytesAvailable();
    */

    // 界面设置
    ui->recvProgressBar->setMaximum(myTotalBytes);
    ui->recvProgressBar->setValue(myRcvedBytes);
    double transpeed = myRcvedBytes / usedTime;
    ui->rfileSizeLineEdit->setText(myTotalBytes / (1024 * 1024) + "MB");
    ui->recvSizeLineEdit->setText(myRcvedBytes / (1024 * 1024) + "MB");
    ui->rateLable->setText(tr("%1").arg(transpeed * 1000 / (1024 * 1024), 0, 'f', 2) + "MB/s");

    if (myRcvedBytes == myTotalBytes) // 文件接收完毕
    {
        myLocalPathFile->close();
        myCntSocket->close();
        ui->rateLable->setText("Completed.");
    }
}

void FileCntDlg::on_cntClosePushButton_clicked()
{
    myCntSocket->abort();
    myLocalPathFile->close();
    close();
}

FileCntDlg::~FileCntDlg()
{
    delete ui;
}
