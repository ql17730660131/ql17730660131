#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setChatName(QString strName)
{
    m_strChatname=strName;
    ui->label->setText(strName);
    m_strLoginname=TCPClient::getInstance().loginName();
}

void PrivateChat::updateMsg(const PDU *pdu)
{
    if(pdu==NULL)
        return;
    char sendName[32];
    memcpy(sendName,pdu->caData,32);
//    char  t[1000];
//    qDebug()<<"接收数据为：";
//    memcpy(t,pdu->caMsg,pdu->uiMsgLen);
//    qDebug()<<t;
    QString str=QString("%1 says: %2").arg(sendName).arg((char*)pdu->caMsg);
    ui->showMsg_te->append(str);
}

void PrivateChat::on_sendMsg_pb_clicked()
{
    QString strMsg=ui->inputMsg_pte->toPlainText();
    if(!strMsg.isEmpty())
    {
        if(strMsg.toStdString().size()>=1000)
        {
            QMessageBox::information(&PrivateChat::getInstance(),"","发送内容过长！");
            return;
        }
        qDebug()<<strMsg;
        PDU *pdu=mkPDU(strMsg.toStdString().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData,m_strLoginname.toStdString().c_str(),m_strLoginname.toStdString().size());
        memcpy(pdu->caData+32,m_strChatname.toStdString().c_str(),m_strChatname.toStdString().size());

        memcpy((char*)pdu->caMsg,strMsg.toStdString().c_str(),strMsg.toStdString().size());
        TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        QString strshow=QString("out:")+strMsg;
        ui->showMsg_te->append(strshow);
        ui->inputMsg_pte->clear();
    }
    else
    {
        QMessageBox::information(&PrivateChat::getInstance(),"私聊","发送内容不能为空！");
    }

}
