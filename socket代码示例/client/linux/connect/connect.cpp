// client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <assert.h>
int main(int argc, const char **argv)
{
    if (argc < 3)
    {
        std::cout << "format: command <ip> <port>" << std::endl;
        exit(0);
    }
    std::string ip(argv[1]);
    int port = std::stoi(argv[2]);

    std::cout << "ip: " << ip << " "
              << "prot: " << port << std::endl;

    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // 大端端口
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect");
        exit(0);
    }
    std::cout << "connect success..." << std::endl;
    // 3. 和服务器端通信
    int number = 0;
    while (1)
    {
        // 发送数据
        std::string str;
        std::cin >> str;
        write(fd, str.c_str(), str.size());

        // 接收数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int len = read(fd, buf, sizeof(buf));
        if (len > 0)
        {
            printf("server: %s\n", buf);
        }
        else if (len == 0)
        {
            printf("服务器断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }
    close(fd);

    return 0;
}