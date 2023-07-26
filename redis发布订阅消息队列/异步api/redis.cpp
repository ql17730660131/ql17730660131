#include "redis.hpp"
#include <iostream>
#include <functional>
#include <string.h>
#include <assert.h>
using namespace std;

Redis::Redis()
    : _publish_context(nullptr), _subcribe_context(nullptr),
      _event_base(0), _event_thread(0)
{
}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }

    if (_subcribe_context != nullptr)
    {
        redisAsyncFree(_subcribe_context);
    }
}
bool Redis::init()
{
    _event_base = event_base_new(); // 创建libevent对象
    if (NULL == _event_base)
    {
        printf(": Create redis event failed.\n");
        return false;
    }

    memset(&_event_sem, 0, sizeof(_event_sem));
    int ret = sem_init(&_event_sem, 0, 0);
    if (ret != 0)
    {
        printf(": Init sem failed.\n");
        return false;
    }

    return true;
}
// 反初始化
bool Redis::uninit()
{
    if (_event_base)
    {
        event_base_free(_event_base);
        _event_base = NULL;
    }
    sem_destroy(&_event_sem);
    return true;
}
bool Redis::connect()
{
    // 负责publish发布消息的上下文连接
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _publish_context)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }
    // else
    {
        // string _password = "ql123456";
        auto reply = (redisReply *)redisCommand(_publish_context, "AUTH %s", "ql123456");
        if (reply->type == REDIS_REPLY_ERROR)
        {
            printf("publish Redis认证失败!\n");
        }
        else
        {
            printf("publish Redis认证成功!\n");
        }
    }

    cout << "publish_context connect redis-server success!" << endl;

    // 负责subscribe订阅消息的上下文连接
    _subcribe_context = redisAsyncConnect("127.0.0.1", 6379);
    if (nullptr == _subcribe_context)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    // attach the event
    // 将事件绑定到redis context上，使设置给redis的回调跟事件关联
    redisLibeventAttach(_subcribe_context, _event_base);

    // 创建事件处理线程
    int ret = pthread_create(&_event_thread, 0, &Redis::event_thread, this);
    if (ret != 0)
    {
        printf(": create event thread failed.\n");
        disconnect();
        return false;
    }

    // 设置连接回调，当异步调用连接后，服务器处理连接请求结束后调用，通知调用者连接的状态
    redisAsyncSetConnectCallback(_subcribe_context,
                                 &Redis::connect_callback);
    // 设置断开连接回调，当服务器断开连接后，通知调用者连接断开，调用者可以利用这个函数实现重连
    redisAsyncSetDisconnectCallback(_subcribe_context,
                                    &Redis::disconnect_callback);
    // 启动事件线程
    sem_post(&_event_sem);
    return true;
}
// 断开连接
bool Redis::disconnect()
{
    if (_subcribe_context)
    {
        redisAsyncDisconnect(_subcribe_context);
        redisAsyncFree(_subcribe_context);
        _subcribe_context = NULL;
    }
    if (_publish_context)
    {
        redisFree(_publish_context);
        _publish_context = NULL;
    }

    return true;
}
// 向redis指定的通道channel发布消息
bool Redis::publish(string channel, string message)
{
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %s %s",
                                                   channel.c_str(), message.c_str());
    if (nullptr == reply)
    {
        std::cout << "publish command failed!" << std::endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向redis指定的通道subscribe订阅消息
bool Redis::subscribe(string channel)
{
    cout << "订阅消息...\n";

    int ret = redisAsyncCommand(this->_subcribe_context,
                                &Redis::command_callback, this, "SUBSCRIBE %s",
                                channel.c_str());
    if (REDIS_ERR == ret)
    {
        printf("Subscribe command failed: %d\n", ret);
        return false;
    }

    printf("Subscribe success: 通道：%s\n", channel.c_str());
    return true;
}

void Redis::init_notify_handler(function<void(string, string)> fn)
{
    this->_notify_message_handler = fn;
}

// 消息接收回调函数
void Redis::command_callback(redisAsyncContext *redis_context,
                             void *reply, void *privdata)
{
    // cout << "command_callback...\n";
    if (NULL == reply)
    {
        return;
    }

    // 静态函数中，要使用类的成员变量，把当前的this指针传进来，用this指针间接访问
    Redis *self_this = reinterpret_cast<Redis *>(privdata);
    redisReply *redis_reply = reinterpret_cast<redisReply *>(reply);

    // 订阅接收到的消息是一个带三元素的数组,且redis_reply->element[2] != nullptr && redis_reply->element[2]->str != nullptr
    if (redis_reply->type == REDIS_REPLY_ARRAY &&
        redis_reply->element[2] != nullptr && redis_reply->element[2]->str != nullptr)
    {
        cout << "通道有消息...\n";
        printf("Channel:%s message:%s\n",
               redis_reply->element[1]->str, redis_reply->element[2]->str);
        if (self_this->_notify_message_handler)
        {
            self_this->_notify_message_handler(redis_reply->element[1]->str, redis_reply->element[2]->str);
        }
    }
}
void Redis::connect_callback(const redisAsyncContext *redis_context, int status)
{
    if (status != REDIS_OK)
    {
        printf("connect redis failed, Error: %s\n", redis_context->errstr);
    }
    else
    {
        auto ret = redisAsyncCommand((redisAsyncContext *)redis_context,
                                     nullptr, nullptr, "AUTH %s", "ql123456");
        if (REDIS_ERR == ret)
        {
            printf("subcribe Redis认证失败!\n");
        }
        else
        {
            printf("subcribe Redis认证成功!\n");
        }
    }
    cout << "_subcribe_context connect redis-server success!" << endl;
}

void Redis::disconnect_callback(const redisAsyncContext *redis_context,
                                int status)
{
    if (status != REDIS_OK)
    {
        // 这里异常退出，可以尝试重连
        printf(": Error: %s\n", redis_context->errstr);
    }
}

void *Redis::event_thread(void *data)
{
    if (NULL == data)
    {
        printf(": Error!\n");
        assert(false);
        return NULL;
    }

    Redis *self_this = reinterpret_cast<Redis *>(data);
    return self_this->event_proc();
}

void *Redis::event_proc()
{
    sem_wait(&_event_sem);
    //  开启事件分发，event_base_dispatch会阻塞
    event_base_dispatch(_event_base);

    return NULL;
}