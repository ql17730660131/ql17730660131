#include <iostream>
#include <string>

#include "ql_rpcapplication.h"
#include "rpcprovider.h"
#include "user.pb.h"

class UserService : public user::UserServiceRpc // 使用在rpc服务发布端
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing service: Login" << std::endl;
        std::cout << "name: " << name << " pwd:" << pwd << std::endl;
        return false;
    }

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id: " << id << " name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    // 重写基类UserServiceRpc的虚函数 下面这些方法都是框架直接调用的
    void Login(::google::protobuf::RpcController *controller,
               const ::user::LoginRequest *request,
               ::user::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 获取相应数据
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool login_result = Login(name, pwd);

        // 把响应写入  包括错误码、错误消息、返回值
        user::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_sucess(login_result);

        // 执行回调操作   执行响应对象数据的序列化和网络发送（都是由框架来完成）
        done->Run();
    }

    void Register(::google::protobuf::RpcController *controller,
                  const ::user::RegisterRequest *request,
                  ::user::RegisterResponse *response,
                  ::google::protobuf::Closure *done)
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Register(id, name, pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_sucess(ret);

        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作
    QlrpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象。把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点 Run以后 进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}