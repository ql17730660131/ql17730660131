#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QByteArray>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>
TCPServer::TCPServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TCPServer)
{
    ui->setupUi(this);
    LoadConfig();

    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TCPServer::~TCPServer()
{
    delete ui;
}

void TCPServer::LoadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray bstr=file.readAll();
        QString str=bstr.toStdString().c_str();
        file.close();
        str.replace("\r\n"," ");
        QStringList strList=str.split(" ");
        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
        qDebug()<<"ip:"<<m_strIP<<" port:"<<m_usPort;
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}
