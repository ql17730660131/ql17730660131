/*** 
 **文 件 名:  
 **说    明:  
 **作    者: cxh
 **版    本: V1.0
 **FilePath: \code\code\test\dir2\socket\logqueue.h
 **时    间: 
 **Date: 2023-07-14 15:20:25
 **LastEditors: cxh
 **LastEditTime: 2023-07-14 15:20:26
 **版权声明：Copyright (c) by racobit, All Rights Reserved
 **修改内容:  
 */
#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 异步写日志的日志队列
template <typename T>
class LogQueue
{
public:
    // 多个worker线程都会写日志queue
    void Push(const T &data);

    // 一个线程读日志queue，写日志文件
    T Pop();

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};

template <typename T>
void LogQueue<T>::Push(const T &data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(data);
    m_condvariable.notify_one();
}

// 一个线程读日志queue，写日志文件
template <typename T>
T LogQueue<T>::Pop()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_queue.empty())
    {
        // 日志队列为空，线程进入wait状态
        m_condvariable.wait(lock);
    }

    T data = m_queue.front();
    m_queue.pop();
    return data;
}