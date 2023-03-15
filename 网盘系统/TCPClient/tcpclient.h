#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"
namespace Ui {
class TCPClient;
}

class TCPClient : public QWidget
{
    Q_OBJECT

public:
    explicit TCPClient(QWidget *parent = 0);
    ~TCPClient();
    void LoadConfig();

    static TCPClient& getInstance();
    QTcpSocket& getTcpSocket();
    QString loginName();
    QString curPath();
    void setCurPath(QString str);
    QString DownLoadPath();


private:
    Ui::TCPClient *ui;
    QString m_strIP;
    quint16 m_usPort;
    //连接服务器，和服务器进行数据交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;
    QString m_strCurPath;
    QString m_strDownLoadPath;

    //下载文件用的
    bool m_downLoad;
    QFile m_file;
    quint32 m_iTotal;
    quint32 m_iRecved;


public slots:
    void showConnect();
    void recvMsg();
private slots:
//    void on_send_pb_clicked();
    void on_login_pb_clicked();
    void on_regist_pb_clicked();
    void on_cancel_pb_clicked();
};

#endif // TCPCLIENT_H
