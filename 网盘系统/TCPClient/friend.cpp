#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include "privatechat.h"
Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit(this);
//    qDebug()<<"m_pShowMsgTE的parent为："<<m_pShowMsgTE->parent();
    m_pFriendListWidget = new QListWidget(this);
    m_pInputMsgLE = new QLineEdit(this);

    m_pDelFriendPB = new QPushButton("删除好友",this);
    m_pFlushFriendPB = new QPushButton("刷新好友",this);
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户",this);
    m_pSearchUsrPB = new QPushButton("查找用户",this);
    m_pMsgSendPB = new QPushButton("发送(群聊)",this);
    m_pPrivateChatPB = new QPushButton("私聊",this);

    QVBoxLayout *pLeftPBVBL =new QVBoxLayout ;
//    qDebug()<<"pLeftPBVBL的parent为："<<pLeftPBVBL->parent();
    pLeftPBVBL->addWidget(m_pDelFriendPB);
    pLeftPBVBL->addWidget(m_pFlushFriendPB);
    pLeftPBVBL->addWidget(m_pShowOnlineUsrPB);
    pLeftPBVBL->addWidget(m_pSearchUsrPB);
    pLeftPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pLeftPBVBL);
    m_pShowMsgTE->setMinimumWidth(400);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online(this);

    QVBoxLayout *pMain =new QVBoxLayout(this) ;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);
    setMinimumHeight(500);
    setMinimumWidth(700);


    connect(m_pShowOnlineUsrPB,SIGNAL(clicked()),this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked()),this,SLOT(SearchUsr()));
    connect(m_pFlushFriendPB,SIGNAL(clicked()),this,SLOT(flushFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked()),this,SLOT(deleteFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked()),this,SLOT(privateChat()));
    connect(m_pMsgSendPB,SIGNAL(clicked()),this,SLOT(groupChat()));
}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu == NULL)
        return;
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu == NULL)
        return;
    uint uiSize=pdu->uiMsgLen/32;
    char caTemp[32];
    m_pFriendListWidget->clear();
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caTemp,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caTemp);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    if(pdu==NULL)
        return;
    char sendName[32];
    memcpy(sendName,pdu->caData,32);
    char  t[1000];
    memcpy(t,pdu->caMsg,pdu->uiMsgLen);
    QString str=QString("%1 says: %2").arg(sendName).arg(t);
    m_pShowMsgTE->append(str);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();

        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }
    else
    {
        m_pOnline->hide();
    }
}

void Friend::SearchUsr()
{
    m_strSearchName=QInputDialog::getText(this,"搜索用户","用户名：");
    if(!m_strSearchName.isEmpty())
    {
        qDebug()<<"搜索用户："<<m_strSearchName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        strcpy(pdu->caData,m_strSearchName.toStdString().c_str());
//        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::flushFriend()
{
    QString strName=TCPClient::getInstance().loginName();
    PDU* pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    mempcpy(pdu->caData,strName.toStdString().c_str(),32);
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::deleteFriend()
{
    if(!m_pFriendListWidget->currentItem())//没有选个某个在线用户就直接返回
        return;
    QString strFriendName=m_pFriendListWidget->currentItem()->text();
    qDebug()<<"要删除好友："<<strFriendName;
    QString strSelfName=TCPClient::getInstance().loginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    mempcpy((char*)(pdu->caData),strSelfName.toStdString().c_str(),32);
    mempcpy((char*)(pdu->caData)+32,strFriendName.toStdString().c_str(),32);
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::privateChat()
{
    if(!m_pFriendListWidget->currentItem())//没有选个某个在线用户就直接返回
    {
        QMessageBox::information(&OpeWidget::getInstance(),"","请先选择一位好友！");
        return;
    }
    QString strChatName=m_pFriendListWidget->currentItem()->text();
    qDebug()<<"私聊对象名称："<<strChatName;
    PrivateChat::getInstance().setChatName(strChatName);
    if(PrivateChat::getInstance().isHidden())
    {
        PrivateChat::getInstance().show();
    }
}

void Friend::groupChat()
{
    QString strMsg=m_pInputMsgLE->text();
    if(strMsg.isEmpty())
        return;
    if(strMsg.toStdString().size()>=1000)
    {
        QMessageBox::information(this,"","发送内容过长！");
        return;
    }
    QString strName = TCPClient::getInstance().loginName();
    PDU *pdu=mkPDU(strMsg.toStdString().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.toStdString().size());

    strcpy((char*)pdu->caMsg,strMsg.toStdString().c_str());
    TCPClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
    QString strshow=QString("out:")+strMsg;
    m_pShowMsgTE->append(strshow);
    m_pInputMsgLE->clear();
}
