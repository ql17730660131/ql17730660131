#include <iostream>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <unordered_map>
#include "redis.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
class ChatServer
{
public:
    ChatServer(EventLoop *loop, const InetAddress &listenaddr, const string &nameArg);
    void start();

    unordered_map<int, TcpConnectionPtr> _userConnMap;

private:
    void onConnection(const TcpConnectionPtr &);

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time);

    void notify_handler(int channel, std::string msg);

    TcpServer m_server;
    EventLoop *m_loop;
    Redis m_redis;
};

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenaddr, const string &nameArg)
    : m_server(loop, listenaddr, nameArg)
{
    // 给服务器注册连接的创建与断开回调
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 给服务器注册用户读写事件回调
    m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置服务端的线程数量
    m_server.setThreadNum(4);

    if (m_redis.connect())
    {
        m_redis.init_notify_handler(std::bind(&ChatServer::notify_handler, this, _1, _2));
        cout << "notify_handler init success...\n";
    }
}

void ChatServer::start()
{
    m_server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        _userConnMap[1] = conn;
        if (m_redis.subscribe(2))
        {
            // cout << "订阅成功...\n";
        }
        if (m_redis.subscribe(1))
        {
            // cout << "订阅成功...\n";
        }

        // cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort()
        //      << " state: online" << endl;
    }
    else
    {
        // cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort()
        //      << " state: offline" << endl;
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    cout << "recv data: " << buf << " time: " << time.toString() << endl;

    // 简单使用
    if (m_redis.publish(1, buf))
    {
        cout << "publish..." << endl;
    }
    else
    {
        cout << "publish failed..." << endl;
    }

    //  conn->send(buf);
}

void ChatServer::notify_handler(int channel, std::string msg)
{
    // (*_userConnMap.begin()).second->send(msg);
    cout << "channel: " << channel << " msg: " << msg << endl;
    auto conn = _userConnMap.find(channel);
    if (conn == _userConnMap.end())
    {
    }
    else
    {
        conn->second->send(msg);
    }
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

// g++ -g muduo_server.cpp redis.cpp -omuduo -lmuduo_net -lmuduo_base -lpthread -lhiredis