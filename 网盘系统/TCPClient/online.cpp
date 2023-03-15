#include "online.h"
#include "ui_online.h"
#include <QDebug>
#include "tcpclient.h"
#include <QMessageBox>
#include "opewidget.h"
Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if(pdu==NULL)
        return;
    uint uiSize=pdu->uiMsgLen/32;
    char caTemp[32]={'\0'};
    ui->online_lw->clear();
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caTemp,(char*)(pdu->caMsg)+i*32,32);
        qDebug()<<caTemp;
        ui->online_lw->addItem(caTemp);
    }
}

void Online::on_addFriend_pb_clicked()
{
    if(!ui->online_lw->currentItem())//没有选个某个在线用户就直接返回
        return;
    QString strPerUsrName=ui->online_lw->currentItem()->text();
    if(strPerUsrName == TCPClient::getInstance().loginName())
    {
        QMessageBox::information(&OpeWidget::getInstance(),"","不能添加自己为好友");
        return;
    }
    qDebug()<<"要加好友："<<strPerUsrName;
    QString strLoginName = TCPClient::getInstance().loginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    strncpy(pdu->caData,strPerUsrName.toStdString().c_str(),32);
    strncpy(pdu->caData+32,strLoginName.toStdString().c_str(),32);
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}
