#include "tcpserver.h"
#include <QApplication>
#include <QTextCodec>
#include "opedb.h"
int main(int argc, char *argv[])
{
//    //解决汉字乱码问题
//        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//        QTextCodec::setCodecForLocale(codec); //解决汉字乱码问题

    QApplication a(argc, argv);
    OpeDB::getInstance().init();

    TCPServer w;
    w.show();

    return a.exec();
}
