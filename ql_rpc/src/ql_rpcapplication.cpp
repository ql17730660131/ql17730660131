#include "ql_rpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

RpcConfig QlrpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout << "format: command -i <config_filename>" << std::endl;
}

void QlrpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        // int optopt——当发现无效选项字符之时，getopt()函数或返回'?'字符，或返回':'字符，并且optopt包含了所发现的无效选项字符
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 加载配置文件
    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    // std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
    // std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;
}

QlrpcApplication &QlrpcApplication::GetInstance()
{
    static QlrpcApplication app;
    return app;
}

RpcConfig &QlrpcApplication::GetConfig()
{
    return m_config;
}