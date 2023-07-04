#include "ql_rpcconfig.h"

#include <iostream>

// 负责解析加载配置文件
void RpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *fp = fopen(config_file, "r");
    if (nullptr == fp)
    {
        std::cout << config_file << " is not exist!" << std::endl;
        exit(EXIT_FAILURE); // 代表异常退出
    }

    // 1.先去掉注释以#开头   2.读取正确的配置项 =    3.去掉多余的空格
    while (!feof(fp))
    {
        char buf[512] = {0};
        fgets(buf, 512, fp); // 读取一行数据

        // 去掉字符串前面多余的空格
        std::string read_buf(buf);
        Trim(read_buf);

        // 判断#的注释
        if (read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }

        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            // 配置项不合法
            continue;
        }

        // 将配置信息保存
        std::string key;
        std::string value;

        key = read_buf.substr(0, idx);
        Trim(key); // 去除key后边空格
        int endidx = read_buf.find('\n', idx);
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value); // 去除value前边空格
        m_configMap.insert({key, value});
        std::cout << "key: " << key << "value: " << value << std::endl;
    }
}

// 查询配置项信息
std::string RpcConfig::Load(const std::string &key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return {};
    }
    return it->second;
}

// 去除多余空格
void RpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串后面有空格
        src_buf = src_buf.substr(0, idx + 1);
    }
}
