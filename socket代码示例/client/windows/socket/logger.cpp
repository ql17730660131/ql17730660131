/*** 
 **文 件 名:  
 **说    明:  
 **作    者: cxh
 **版    本: V1.0
 **FilePath: \code\code\test\dir2\socket\logger.cpp
 **时    间: 
 **Date: 2023-07-14 15:19:09
 **LastEditors: cxh
 **LastEditTime: 2023-07-14 15:34:20
 **版权声明：Copyright (c) by racobit, All Rights Reserved
 **修改内容:  
 */
#include "logger.h"
#include <iostream>
#include <time.h>
// 获取日志的单例
Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}

void Logger::Log(std::string msg)
{
    m_logQue.Push(msg);
}

Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&]()
                             {
        for (;;)
        {
            // 获取当前的日期，然后取日志信息，写入相应的日志文件当中
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            //tm_year 从1900年计算，所以要加1900  tm_mon，从0计算，所以要加1
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);

            FILE *fp = fopen(file_name, "a+");
            if (fp == nullptr)
            {
                std::cout << "logger file : " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_logQue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d =>[%s] ", 
                    nowtm->tm_hour, 
                    nowtm->tm_min, 
                    nowtm->tm_sec,
                    (m_loglevel == INFO_ ? "info" : "error"));
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), fp);
            fclose(fp);
        } });
    // 设置分离线程
    writeLogTask.detach();
}
