#ifndef _CONNECTION_
#define _CONNECTION_
#include <mysql/mysql.h>
#include <string>
#include <ctime>
#include <chrono>
using namespace std;
using namespace chrono;
class Connection
{
public:
    // 初始化数据库连接
    Connection();
    // 释放数据库连接资源
    ~Connection();
    // 连接数据库
    bool connect(string ip, unsigned short port, string username, string password, string dbname);
    // 更新操作 insert、delete、update
    bool update(string sql);
    // 查询操作 select
    MYSQL_RES *query(string sql);

    // 刷新一下连接的起始的空闲时间点
    void refreshAliveTime() { m_alivetime = steady_clock::now(); }

    long long getAliveTime();

private:
    MYSQL *m_conn;         // 表示和MySQL Server的一条连接
    steady_clock::time_point m_alivetime;   // 记录进入空闲状态后的起始存活时间
    MYSQL_RES *m_mysqlRes; // 存放获取到的结果集
};

#endif