#include "Connection.h"
#include "public.h"
#include <iostream>
using namespace std;

Connection::Connection()
{
    // 初始化数据库连接
    m_conn = mysql_init(nullptr);
    m_mysqlRes = nullptr;
}
Connection::~Connection()
{
    // 释放数据库连接资源
    if (m_conn != nullptr)
        mysql_close(m_conn);

    // m_mysqlRes不为空释放结果集
    if (m_mysqlRes != nullptr)
    {
        mysql_free_result(m_mysqlRes); // 释放结果集
    }
}

bool Connection::connect(string ip, unsigned short port, string username, string password, string dbname)
{
    // 连接数据库
    MYSQL *p = mysql_real_connect(m_conn, ip.c_str(), username.c_str(),
                                  password.c_str(), dbname.c_str(), port, nullptr, 0);
    return p != nullptr;
}

bool Connection::update(string sql)
{
    // 更新操作 insert、delete、update
    if (mysql_query(m_conn, sql.c_str()))
    {
        LOG("更新失败:" + sql);
        return false;
    }
    return true;
}

MYSQL_RES *Connection::query(string sql)
{
    // m_mysqlRes不为空释放结果集
    if (m_mysqlRes != nullptr)
    {
        mysql_free_result(m_mysqlRes); // 释放结果集
    }
    // 查询操作 select
    if (mysql_query(m_conn, sql.c_str()))
    {
        LOG("查询失败:" + sql);
        return nullptr;
    }
    m_mysqlRes = mysql_store_result(m_conn);
    return m_mysqlRes;
}

long long Connection::getAliveTime()  
{
    nanoseconds res = steady_clock::now() - m_alivetime;
    milliseconds millsec = duration_cast<milliseconds>(res);//纳秒转换为毫秒
    return millsec.count();//count方法返回多少个毫秒
}
