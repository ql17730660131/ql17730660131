#include "Poller.h"
#include "EPollPoller.h"
#include <stdlib.h>

// EventLoop可以通过该接口获取默认的IO复用的具体实现
Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    //没有实现poll版本，故返回epoll实例
    if (::getenv("MUDUO_USE_POLL"))
    {
        return nullptr; // 生成poll的实例
    }
    else
    {
        return new EPollPoller(loop); // 生成epoll的实例
    }
}