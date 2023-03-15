#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TCPClient::getInstance().show();
//    TCPClient w;
//    w.show();
    return a.exec();
}
