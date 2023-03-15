#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "threadpool.h"
#include "http_conn.h"

const int MAX_FD = 65535;           // 客户最大人数
const int MAX_EVENT_NUMBER = 10000; // 监听的最大的数量

// 添加信号捕捉
void addsig(int sig, void (*handler)(int))
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask); // 将sa_mask集合中所有的标志位设置为1
                             //     sa_mask: 在信号处理函数执行期间，临时屏蔽某些信号 , 将要屏蔽的信号设置到集合中即可
                             // 当前处理函数执行完毕，临时屏蔽自动解除
                             // 假设在这个集合中不屏蔽任何信号，默认也会屏蔽一个（捕捉的信号是谁，就临时屏蔽谁）
    sigaction(sig, &sa, NULL);
}

// 添加文件描述符到epoll
extern void addfd(int epollfd, int fd, bool one_shot);
// 从epoll删除文件描述符
extern void removefd(int epollfd, int fd);
int main(int argc, const char **argv)
{
    http_conn::m_user_count.store(0);
    if (argc <= 1)
    {
        std::cout << "请按照以下格式运行：" << basename(argv[0]) << "端口号" << std::endl;
        exit(-1);
    }
    // 获取端口号
    int port = atoi(argv[1]);
    // 对SIGPIE信号进行处理
    addsig(SIGPIPE, SIG_IGN);

    // 创建一个数组，保存所有客户端信息
    http_conn *users = new http_conn[MAX_FD];

    // 1. 创建监听的套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        exit(0);
    }
    // 设置端口复用
    int reuse = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 绑定
    struct sockaddr_in address;
    address.sin_family = PF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int ret = bind(lfd, (struct sockaddr *)&address, sizeof(address));
    if (ret == -1)
    {
        perror("bind");
        exit(0);
    }
    // 设置监听
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        exit(0);
    }

    // 创建epoll对象，事件数组 添加
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(1); // 创建epoll对象
    if (epollfd == -1)
    {
        perror("epoll_create");
        exit(0);
    }

    // 将监听的文件描述符添加到epoll对象中,不使用addfd(),不然会设置为边沿触发
    {
        epoll_event event;
        event.data.fd = lfd;
        event.events = EPOLLIN;
        // EpollErr和EpollHup会自动加上。
        epoll_ctl(epollfd, EPOLL_CTL_ADD, lfd, &event);
    }

    http_conn::m_epollfd = epollfd;

    // 初始化线程池
    qianlan::ThreadPool thread_pool(10); // 默认创建4个线程
    while (true)
    {
        int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if ((num < 0) && (errno != EINTR))
        { // 不是被中断
            std::cout << "epoll failure" << std::endl;
            break;
        }
        std::cout << "num=" << num << std::endl;
        // 循坏遍历数组
        for (int i = 0; i < num; i++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == lfd)
            {
                std::cout << "监听lfd有数据" << std::endl;
                // 有客户端连接进来
                struct sockaddr_in client_address;
                socklen_t client_addrlen = sizeof(client_address);
                int connfd = accept(lfd, (struct sockaddr *)&client_address, &client_addrlen);
                if (connfd == -1)
                {
                    perror("accept");
                    exit(0);
                }
                if (http_conn::m_user_count >= MAX_FD)
                {
                    // 目前连接数已经满了服务器正忙...
                    close(connfd);
                    continue;
                }
                // 打印客户端的地址信息
                char ip[24] = {0};
                printf("客户端的IP地址: %s, 端口: %d\n",
                       inet_ntop(AF_INET, &client_address.sin_addr.s_addr, ip, sizeof(ip)),
                       ntohs(client_address.sin_port));
                // 将新的客户的数据初始化，放到数组中，用文件描述符作为索引操作比较方便
                users[connfd].init(connfd, client_address);
                std::cout << "文件描述符为" <<connfd<< std::endl;
            }
            else if (events[i].events & EPOLLRDHUP || events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
            { // 对方异常断开或者错误等事件
                // EPOLLRDHUP    这个好像有些系统检测不到，可以使用EPOLLIN，read返回0，删除掉事件，关闭close(fd);
                std::cout << "异常断开" << std::endl;
                users[sockfd].close_conn();
            }
            else if (events[i].events & EPOLLIN)
            {
                std::cout << "有读的数据" << std::endl;
                thread_pool.commit(&http_conn::handle_read, &users[sockfd]);
            }
            else if (events[i].events & EPOLLOUT)
            {
                // std::cout << "有写的数据" << std::endl;
                thread_pool.commit(&http_conn::handle_write, &users[sockfd]);
            }
            std::cout << "文件描述符为" <<sockfd<< std::endl;
        }
        std::cout << "连接人数：" << http_conn::m_user_count << std::endl;
    }
    close(epollfd);
    close(lfd);
    delete[] users;
    return 0;
}
