#include "CommonConnectionPool.h"
#include "public.h"
// 线程安全的懒汉单例函数接口
ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

// 从配置文件中加载配置项
bool ConnectionPool::loadConfigFile()
{
    FILE *pf = fopen("mysql.ini", "r");
    if (pf == nullptr)
    {
        LOG("mysql.ini file is not exist!");
        return false;
    }
    // feof用检测流上的文件结束符，其返回值有两种情况：如果遇到文件结束，函数值为非零值，否则函数值为0。
    while (!feof(pf))
    {
        char line[1024] = {0};
        fgets(line, 1024, pf); // 通俗来讲的话，fgets()函数的作用就是用来读取一行数据的
        string str = line;
        int idx = str.find('=', 0);
        if (idx == -1) // 无效的配置项
        {
            continue;
        }
        int endidx = str.find('\n', idx);
        string key = str.substr(0, idx);
        string value = str.substr(idx + 1, endidx - idx - 1);
        if (key == "ip")
        {
            m_ip = value;
        }
        else if (key == "port")
        {
            m_port = atoi(value.c_str());
        }
        else if (key == "username")
        {
            m_username = value;
        }
        else if (key == "password")
        {
            m_password = value;
        }
        else if (key == "dbname")
        {
            m_dbname = value;
        }
        else if (key == "initSize")
        {
            m_initSize = atoi(value.c_str());
        }
        else if (key == "maxSize")
        {
            m_maxSize = atoi(value.c_str());
        }
        else if (key == "maxIdleTime")
        {
            m_maxIdleTime = atoi(value.c_str());
        }
        else if (key == "connectionTimeOut")
        {
            m_connectionTimeout = atoi(value.c_str());
        }
    }
    return true;
}
// 连接池的构造
ConnectionPool::ConnectionPool()
{
    // 加载配置项了
    if (!loadConfigFile())
    {
        LOG("配置文件书写有误");
        return;
    }

    // 创建初始数量的连接
    for (int i = 0; i < m_initSize; ++i)
    {
        Connection *p = new Connection();
        p->connect(m_ip, m_port, m_username, m_password, m_dbname);
        p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
        m_connectionQue.push(p);
        m_connectionCnt++;
    }
    // 启动一个新的线程，作为连接的生产者 linux thread => pthread_create
    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach(); // 进行线程分离

    // 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
    thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}
ConnectionPool::~ConnectionPool()
{
    while (!m_connectionQue.empty())
    {
        Connection *conn = m_connectionQue.front();
        m_connectionQue.pop();
        delete conn;
    }
}
// 运行在独立的线程中，专门负责生产新连接
void ConnectionPool::produceConnectionTask()
{
    // std::cout << "生产连接线程工作中..." << std::endl;
    while (1)
    {
        unique_lock<mutex> lock(m_queueMutex);

        // 都使用while是因为使用一个条件变量，有伪唤醒，是伪唤醒则重新等待，可以使用两个条件变量处理
        while (!m_connectionQue.empty())
        {
            cv.wait(lock); // 队列不空，此处生产线程进入等待状态
        }
        // 连接数量没有到达上限，继续创建新的连接
        if (m_connectionCnt < m_maxSize)
        {
            Connection *p = new Connection();
            p->connect(m_ip, m_port, m_username, m_password, m_dbname);
            p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
            m_connectionQue.push(p);
            m_connectionCnt++;
            std::cout << "产生新连接成功"
                      << "连接数量为：" << m_connectionCnt << std::endl;
            std::cout << "连接队列大小为：" << m_connectionQue.size() << std::endl;
        }
        // 通知消费者线程，可以消费连接了
        cv.notify_all();
    }
}

// 给外部提供接口，从连接池中获取一个可用的空闲连接
shared_ptr<Connection> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(m_queueMutex);
    while (m_connectionQue.empty()) // 都使用while是因为使用一个条件变量，有伪唤醒，是伪唤醒则重新等待
    {
        if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(m_connectionTimeout)))
        {
            if (m_connectionQue.empty())
            {
                LOG("获取空闲连接超时了...获取连接失败!");
                // return nullptr;
                continue;//继续下一次循环
            }
        }
    }
    // shared_ptr智能指针析构时，会把connection资源直接delete掉，相当于调用connection的析构函数，connection就被close掉了
    // 这里需要自定义shared_ptr的释放资源的方式，把connection直接归还到queue当中
    shared_ptr<Connection> sp(m_connectionQue.front(),
                              [this](Connection *pcon)
                              {
                                  // 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
                                  unique_lock<mutex> lock(m_queueMutex);
                                  pcon->refreshAliveTime(); // 刷新一下开始空闲的起始时间
                                  m_connectionQue.push(pcon);
                              });
    m_connectionQue.pop();
    cv.notify_all(); // 消费完连接以后，通知生产者线程检查一下，如果队列为空了，赶紧生产连接
    // std::cout << "获取连接成功" << std::endl;
    return sp;
}

// 扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
void ConnectionPool::scannerConnectionTask()
{
    for (;;)
    {
        // 通过sleep模拟定时效果
        this_thread::sleep_for(chrono::seconds(m_maxIdleTime));
        // 扫描整个队列，释放多余的连接
        std::cout << "定时扫描线程工作中..." << std::endl;
        unique_lock<mutex> lock(m_queueMutex);
        while (m_connectionCnt > m_initSize)
        {
            if (!m_connectionQue.empty())
            {
                // std::cout << "队列非空，准备扫描超时连接..." << std::endl;
                Connection *p = m_connectionQue.front();
                // getAliveTime()获得的是毫秒 m_maxIdleTime是秒 要进行换算
                // std::cout << "AliveTime: " << p->getAliveTime() << std::endl;
                if (p->getAliveTime() >= (m_maxIdleTime * 1000)) // 超过15秒删除
                {
                    m_connectionQue.pop();
                    m_connectionCnt--;
                    delete p;
                    // std::cout << "删除超时连接成功" << std::endl;
                    // std::cout << "连接队列大小为：" << m_connectionQue.size() << std::endl;
                }
                else
                    break; // 队头的连接没有超过m_maxIdleTime，其它连接肯定没有,每次添加进入队列会刷新时间
            }
            else // 对列为空跳出循环
                break;
        }
    }
}