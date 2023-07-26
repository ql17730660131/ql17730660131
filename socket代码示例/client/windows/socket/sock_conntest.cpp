
// 编译时手动添加  -lwsock32参数 见build.txt
#include <stdio.h>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <iostream>
#include <functional>
#include "logger.h"

#pragma comment(lib, "ws2_32.lib")
static void analysis(char *data, int size);

void start(int n);
int main(int argc, char *argv[])
{
    _setmaxstdio(9000);
    // windows一些初始化
    WORD socket_version;
    WSADATA wsadata;
    socket_version = MAKEWORD(2, 2);
    if (WSAStartup(socket_version, &wsadata) != 0)
    {
        printf("WSAStartup error!");
        exit(0);
    }

    std::vector<std::thread> worker;
    for (int i = 0; i < 1000; i++)    //根据需要修改线程数
    {
        worker.emplace_back(std::bind(start,i+1));
        //注意等待时间*线程数小于线程存活时间
        Sleep(5);//等5毫秒  不然大量connect失败

    }
    for (auto &thread : worker)

    {
        thread.join();
    }

    WSACleanup();
    std::cout << "finish-----------------------------------------\n";
    return 0;
}

void start(int n)
{
    SOCKET fd;               // 本地创建的socket
    struct sockaddr_in addr; // 用于存储服务器的基本信息

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == INVALID_SOCKET)
    {
        printf("invalid socket !\n");
        LOG_INFO("invalid socket !.............................");
        // exit(0);
        return;
    }

    addr.sin_family = AF_INET;                             // IPV4协议族
    addr.sin_port = htons(10000);                          // 服务器的端口号
    addr.sin_addr.S_un.S_addr = inet_addr("43.136.243.4"); // 服务IP
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        printf("connect error\n");
        LOG_ERR("connect error %d",fd);
        // exit(0);
        return;
    }

    printf("connect %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port); // 打印服务器信息

    int ret = 0;
    int count = 0;
    while (1)
    {
        if (count++ >= 10)
            break;

        char buf[1024];
        memset(buf, 0, sizeof buf);
        sprintf(buf, "hello i am client %d...", n);

        ret = send(fd, buf, strlen(buf), 0);
        if (ret <= 0)
        {
            LOG_INFO("send error.............................");
            break;
        }

        memset(buf, 0, sizeof buf);
        ret = recv(fd, buf, sizeof buf, 0);

        if (ret > 0)
        {
            analysis(buf, ret);
            Sleep(2000);
        }
        else if (ret == 0)
        {
            // 当ret == 0 说明服务器掉线
            printf("lost connection , Ip = %s\n", inet_ntoa(addr.sin_addr));
            LOG_INFO("lost connection.............................");
            break;
        }
        else // ret < 0
        {
            // 当ret < 0 说明出现了异常 例如阻塞状态解除，或者读取数据时出现指针错误等。
            // 所以我们这里要主动断开和客户端的链接。
            printf("something wrong of %s\n", inet_ntoa(addr.sin_addr));
            LOG_ERR("something wrong.............................");
            break;
        }
    }
    // 关闭套接字
    if (closesocket(fd))
    {
        std::cout << "closesocket error..." << std::endl;
        LOG_ERR("closesocket error.............................");
    }
}

static void analysis(char *data, int size)
{
    printf("recv data:%s  size:%d\n", data, size);
    // 在这里我们可以对已接收到的数据进行处理
}