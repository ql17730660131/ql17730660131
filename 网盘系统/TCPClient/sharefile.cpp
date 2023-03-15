#include "sharefile.h"
#include "ui_sharefile.h"
#include "tcpclient.h"
#include "opewidget.h"
ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::updateFriend(QListWidget *listWidget)
{
    if(listWidget==NULL)
        return;
    ui->listWidget->clear();
    for(int i=0;i<listWidget->count();i++)
    {
        QString str = listWidget->item(i)->text();
        ui->listWidget->addItem(str);
    }
}

void ShareFile::on_cancel_pb_clicked()
{
    this->close();
}

void ShareFile::on_ok_pb_clicked()
{
    if(!ui->listWidget->currentItem())
        return;
    QString strName = TCPClient::getInstance().loginName();
    QString curPath = TCPClient::getInstance().curPath();
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();
    QString strPath = curPath+"/"+strShareFileName;
    qDebug()<<"分享文件路径"<<strPath;
    QString strRcvName = ui->listWidget->currentItem()->text();
    qDebug()<<"接收者名字："<<strRcvName;
    PDU *pdu=mkPDU(strPath.toStdString().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    memcpy(pdu->caData,strRcvName.toStdString().c_str(),strRcvName.toStdString().size());
    memcpy(pdu->caData+32,strName.toStdString().c_str(),strName.toStdString().size());
    memcpy(pdu->caMsg,strPath.toStdString().c_str(),strPath.toStdString().size());
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
    this->hide();

}
