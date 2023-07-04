#include <iostream>
#include "ql_rpcapplication.h"
#include "user.pb.h"
#include "ql_rpcchannel.h"

int main(int argc, char **argv)
{
    // 要使用ql_rpc框架来进行rpc服务调用，先调用框架的初始化函数
    QlrpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    user::UserServiceRpc_Stub stub(new QlrpcChannel());
    // rpc方法的请求参数
    user::LoginRequest request;
    request.set_name("qian lan");
    request.set_pwd("123456");
    // rpc方法的响应
    user::LoginResponse response;

    // 发起rpc方法的调用  同步的rpc调用过程  
    // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    stub.Login(nullptr, &request, &response, nullptr); 

    // 一次rpc调用完成，读调用的结果
    if (0 == response.result().errcode())
    {
        std::cout << "rpc login response success:" << response.sucess() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
    }

    // 演示调用远程发布的rpc方法Register
    user::RegisterRequest req;
    req.set_id(2000);
    req.set_name("ql_rpc");
    req.set_pwd("666666");
    user::RegisterResponse rsp;

    // 以同步的方式发起rpc调用请求，等待返回结果
    stub.Register(nullptr, &req, &rsp, nullptr); 

    //rpc调用完成，读调用的结果
    if (0 == rsp.result().errcode())
    {
        std::cout << "rpc register response success:" << rsp.sucess() << std::endl;
    }
    else
    {
        std::cout << "rpc register response error : " << rsp.result().errmsg() << std::endl;
    }
    
    return 0;
}