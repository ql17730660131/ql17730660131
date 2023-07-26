/*** 
 **文 件 名:  
 **说    明:  
 **作    者: cxh
 **版    本: V1.0
 **FilePath: \code\code\test\dir2\socket\logger.h
 **时    间: 
 **Date: 2023-07-14 15:18:58
 **LastEditors: cxh
 **LastEditTime: 2023-07-14 15:30:05
 **版权声明：Copyright (c) by racobit, All Rights Reserved
 **修改内容:  
 */
#pragma once

#include "logqueue.h"
#include <string>


// 定义宏进行日志输出
#define LOG_INFO(logmsgformat, ...)                       \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(LogLevel::INFO_);                         \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
    } while (0)

#define LOG_ERR(logmsgformat, ...)                        \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(LogLevel::ERROR_);                        \
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