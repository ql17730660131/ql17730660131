#include <iostream>
#include <ql_muduo/TcpServer.h>
#include <ql_muduo/EventLoop.h>
#include <functional>
#include "redis.hpp"
using namespace std;
using namespace placeholders;
class ChatServer
{
public:
    ChatServer(EventLoop *loop, const InetAddress &listenaddr, const string &nameArg);
    ~ChatServer();
    void start();

    unordered_map<string, TcpConnectionPtr> _userConnMap;

private:
    void
    onConnection(const TcpConnectionPtr &);

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time);

    void notify_handler(string channel, std::string msg);

    TcpServer m_server;
    EventLoop *m_loop;
    Redis m_redis;
    ;
};

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenaddr, const string &nameArg)
    : m_server(loop, listenaddr, nameArg)
{
    // 给服务器注册连接的创建与断开回调
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, placeholders::_1));

    // 给服务器注册用户读写事件回调
    m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // m_server.setThreadInitcallback([](EventLoop *loop){
    //     std::cout<<loop<<std::endl;
    //     std::cout<<"线程创建成功..."<<std::endl;
    // });
    // 设置服务端的线程数量
    m_server.setThreadNum(4);

    if (m_redis.init())
    {
        cout << "m_redis init success...\n";
    }
    if (m_redis.connect())
    {
        m_redis.init_notify_handler(std::bind(&ChatServer::notify_handler, this, _1, _2));
        cout << "notify_handler init success...\n";
    }
}
ChatServer::~ChatServer()
{
    cout << "服务器终止..." << endl;
}
void ChatServer::start()
{
    m_server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        _userConnMap["2"] = conn;
        if (m_redis.subscribe("2"))
        {
            cout << "订阅成功...\n";
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
    // short size = buf.size();
    //    buf.insert(0, std::string("浅蓝"));
    // cout << "send data: " << buf << " buf: " << time.toString() << endl;

    // 简单使用
    // m_redis.subscribe(1);
    m_redis.publish("1", buf);
    cout << "publish..." << endl;
    // conn->send(buf);
}

void ChatServer::notify_handler(string channel, std::string msg)
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
    InetAddress addr("0.0.0.0", 10001);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}

// g++ ql_muduo_server.cpp redis.cpp -oql_muduo -lql_muduo -lpthread -lhiredis