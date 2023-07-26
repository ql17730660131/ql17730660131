#pragma once

#include "logqueue.h"
#include <string>

// 定义宏进行日志输出
#define LOG_INFO(logmsgformat, ...)                       \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(INFO_);                         \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
    } while (0)

#define LOG_ERR(logmsgformat, ...)                        \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(ERROR_);                        \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
    } while (0)

// 定义日志级别
enum LogLevel
{
    INFO_,  // 普通信息
    ERROR_, // 错误信息
};

// ql_rpc框架提供的日志系统
class Logger
{
public:
    // 获取日志的单例
    static Logger &GetInstance();
    // 设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg);

private:
    int m_loglevel;                 // 记录日志级别
    LogQueue<std::string> m_logQue; // 日志缓冲队列

    Logger();
    Logger(const Logger &obj) = delete;
    Logger &operator=(const Logger &obj) = delete;
};