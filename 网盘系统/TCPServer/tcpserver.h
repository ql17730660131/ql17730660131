#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include "mytcpserver.h"

namespace Ui {
class TCPServer;
}

class TCPServer : public QWidget
{
    Q_OBJECT

public:
    explicit TCPServer(QWidget *parent = 0);
    ~TCPServer();

    void LoadConfig();
private:
    Ui::TCPServer *ui;

    QString m_strIP;
    quint16 m_usPort;
};

#endif // TCPSERVER_H
