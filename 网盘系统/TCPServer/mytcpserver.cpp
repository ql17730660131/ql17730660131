#include "mytcpserver.h"

MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::resend(const char *perName, PDU *pdu)
{
    if(NULL==perName || pdu==NULL)
        return;
    QString strName=perName;
    for(int i=0;i<m_tcpSocketList.size();i++)
    {
        if(m_tcpSocketList.at(i)->getName()==strName)
        {
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*))
            , this, SLOT(deleteSocket(MyTcpSocket*)));

}

void MyTcpServer::deleteSocket(MyTcpSocket *mySocket)
{ 
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for (; iter != m_tcpSocketList.end(); iter++)
    {
        if (mySocket == *iter)
        {
            (*iter)->deleteLater();//直接delete会出异常退出
            qDebug()<<(*iter)->getName()<<"下线";
            m_tcpSocketList.erase(iter);

            qDebug()<<"剩余在线人数"<<m_tcpSocketList.size();
            break;
        }
    }

    for(int i=0;i<m_tcpSocketList.size();i++)
    {
        qDebug()<<"打印登录名称：";
        qDebug()<< m_tcpSocketList.at(i)->getName()<<"在线";
    }
}
