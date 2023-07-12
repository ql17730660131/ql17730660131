#安装
直接执行环境配置编译安装脚本文件autobuild.sh
#使用
服务端：
包含头文件
    #include <ql_muduo/TcpServer.h>
    #include <ql_muduo/EventLoop.h>

    TcpServer server;
    server.setThreadNum(n);//n为设置的线程数
    设置回调
    server.start();

    EventLoop loop;
    loop.loop();
##简单示例
#include <iostream>
#include <ql_muduo/TcpServer.h>
#include <ql_muduo/EventLoop.h>
#include <functional>
using namespace std;
class ChatServer
{
public:
    ChatServer(EventLoop *loop, const InetAddress &listenaddr, const string &nameArg);
    void start();

private:
    void onConnection(const TcpConnectionPtr &);

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time);

    TcpServer m_server;
    EventLoop *m_loop;
};

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenaddr, const string &nameArg)
    : m_server(loop, listenaddr, nameArg)
{
    // 给服务器注册连接的创建与断开回调
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, placeholders::_1));

    // 给服务器注册用户读写事件回调
    m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // 设置服务端的线程数量
    m_server.setThreadNum(2);
}

void ChatServer::start()
{
    m_server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort()
             << " state: online" << endl;
    }
    else
    {
        cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort()
             << " state: offline" << endl;
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    cout << "recv data: " << buf << " time: " << time.toString() << endl;
    conn->send(buf);
}

int main()
{
    EventLoop loop;
    InetAddress addr("0.0.0.0", 10000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}
