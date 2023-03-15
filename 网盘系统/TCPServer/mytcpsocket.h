#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"
#include <QDir>
#include <QTimer>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    ~MyTcpSocket();
    QString getName();
signals:
    void offline(MyTcpSocket* mySocket);

public slots:
    void recvMsg();
    void clientOffline();
    void sendFileToClient();
private:
    QString m_strName;//用户名

    QFile m_file;
    quint32 m_iTotal;
    quint32 m_iRecved;
    bool m_upLoad;

    QTimer m_Timer;

};

#endif // MYTCPSOCKET_H
