#ifndef _COMMONCONNECTIONPOOL_
#define _COMMONCONNECTIONPOOL_
#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
#include "Connection.h"
using namespace std;

// 实现连接池功能模块
class ConnectionPool
{
public:
    // 获取连接池对象实例
    static ConnectionPool *getConnectionPool();
    // 给外部提供接口，从连接池中获取一个可用的空闲连接
    shared_ptr<Connection> getConnection();

    // 资源释放
    ~ConnectionPool();

private:
    // 单例#1 构造函数私有化
    ConnectionPool();

    // 从配置文件中加载配置项
    bool loadConfigFile();

    // 运行在独立的线程中，专门负责生产新连接
    void produceConnectionTask();

    // 扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
    void scannerConnectionTask();

    string m_ip;           // mysql的ip地址
    unsigned short m_port; // mysql的端口号 3306
    string m_username;     // mysql登录用户名
    string m_password;     // mysql登录密码
    string m_dbname;       // 连接的数据库名称

    int m_initSize;          // 连接池的初始连接量
    int m_maxSize;           // 连接池的最大连接量
    int m_maxIdleTime;       // 连接池最大空闲时间
    int m_connectionTimeout; // 连接池获取连接的超时时间

    queue<Connection *> m_connectionQue; // 存储mysql连接的队列
    mutex m_queueMutex;                  // 维护连接队列的线程安全互斥锁
    atomic_int m_connectionCnt;          // 记录连接所创建的connection连接的总数量
    condition_variable cv;               // 设置条件变量，用于连接生产线程和连接消费线程的通信
};

#endif