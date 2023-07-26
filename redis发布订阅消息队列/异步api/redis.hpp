#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <hiredis/async.h>
#include <functional>
#include <semaphore.h>
#include <hiredis/adapters/libevent.h>
using namespace std;

class Redis
{
public:
    Redis();
    ~Redis();

    // 连接redis服务器
    bool connect();

    // 断开连接
    bool disconnect();
    // 初识化
    bool init();
    // 反初始化
    bool uninit();
    // 向redis指定的通道channel发布消息
    bool publish(string channel, string message);

    // 向redis指定的通道subscribe订阅消息
    bool subscribe(string channel);

    // 向redis指定的通道unsubscribe取消订阅消息
    bool unsubscribe(int channel);

    // 在独立线程中接收订阅通道中的消息
    void observer_channel_message();

    // 初始化向业务层上报通道消息的回调对象
    void init_notify_handler(function<void(string, string)> fn);

    static void command_callback(redisAsyncContext *redis_context, void *reply, void *privdata);
    // 连接的回调
    static void connect_callback(const redisAsyncContext *redis_context, int status);
    // 断开连接的回调
    static void disconnect_callback(const redisAsyncContext *redis_context, int status);
    // 事件分发线程函数
    static void *event_thread(void *data);

    void *event_proc();

private:
    // libevent事件对象
    event_base *_event_base;

    //  事件线程ID
    pthread_t _event_thread;

    //  事件线程的信号量
    sem_t _event_sem;
    // 负责publish消息
    redisContext *_publish_context;

    // 负责subscribe消息
    redisAsyncContext *_subcribe_context;

    // 回调操作，收到订阅的消息，给service层上报
    function<void(string, string)> _notify_message_handler;
};

#endif
