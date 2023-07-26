// client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <thread>
#include <functional>
#include <vector>
#include "logger.h"//打印太快 故而打印信息到文件
void start(int n);

int main()
{
    std::vector<std::thread> worker;
    for (int i = 0; i < 5000; i++)
    {
        worker.emplace_back(std::bind(start, i + 1));
    }
    for (auto &thread : worker)
    {
        thread.join();
    }
    return 0;
}

void start(int n)
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("invalid socket...");
        LOG_ERR("invalid socket...");
        return;
    }

    // 2. 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000); // 大端端口
    inet_pton(AF_INET, "43.136.243.4", &addr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect");
        LOG_ERR("connect");
        return;
    }

    // 3. 和服务器端通信
    int number = 0;
    while (1)
    {
        if (number++ >= 10)
            break;
        // 发送数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "你好, 服务器...fd = %d\n", n);
        write(fd, buf, strlen(buf) + 1);

        // 接收数据
        memset(buf, 0, sizeof(buf));
        int len = read(fd, buf, sizeof(buf));
        if (len > 0)
        {
            printf("服务器say: %s\n", buf);
        }
        else if (len == 0)
        {
            printf("服务器断开了连接...\n");
            LOG_INFO("服务器断开了连接...");
            break;
        }
        else
        {
            perror("read");
            LOG_ERR("read...");
            break;
        }
        sleep(1); // 每隔1s发送一条数据
    }
    close(fd);
}
