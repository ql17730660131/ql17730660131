#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"
class MyTcpServer : public QTcpServer
{
    Q_OBJECT//用于支持信号与槽
public:
    MyTcpServer();

    static MyTcpServer& getInstance();

    void resend(const char* perName,PDU* pdu);//转发添加好友信息

protected:
    void incomingConnection(qintptr socketDescriptor);
public slots:
    void deleteSocket(MyTcpSocket* mySocket);
private:
    QList<MyTcpSocket*> m_tcpSocketList;

};

#endif // MYTCPSERVER_H
