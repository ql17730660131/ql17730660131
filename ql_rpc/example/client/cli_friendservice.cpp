#include <iostream>
#include "ql_rpcapplication.h"
#include "friend.pb.h"
#include <ql_rpccontroller.h>

int main(int argc, char **argv)
{
    // 要使用ql_rpc框架来进行rpc服务调用，先调用框架的初始化函数
    QlrpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法GetFriendsList
    ::friend_::FiendServiceRpc_Stub stub(new QlrpcChannel());
    // rpc方法的请求参数
    ::friend_::GetFriendsListRequest request;
    request.set_userid(1000);
    // rpc方法的响应
    ::friend_::GetFriendsListResponse response;
    // 发起rpc方法的调用  同步的rpc调用过程
    // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    QlrpcController controller;
    stub.GetFriendsList(&controller, &request, &response, nullptr); 

    // 一次rpc调用完成，读调用的结果
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "rpc GetFriendsList response success!" << std::endl;
            int size = response.friends_size();
            for (int i=0; i < size; ++i)
            {
                std::cout << "index:" << (i+1) << " name:" << response.friends(i) << std::endl;
            }
        }
        else
        {
            std::cout << "rpc GetFriendsList response error : " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}