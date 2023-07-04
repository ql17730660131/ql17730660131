#pragma once

#include "ql_rpcconfig.h"
#include "ql_rpcchannel.h"
// #include "ql_rpccontroller.h"

// mprpc框架的基础类，负责框架的一些初始化操作
class QlrpcApplication
{
public:
    static void Init(int argc, char **argv);
    static QlrpcApplication &GetInstance();
    static RpcConfig &GetConfig();

private:
    static RpcConfig m_config;

    QlrpcApplication() {}
    QlrpcApplication(const QlrpcApplication &) = delete;
    QlrpcApplication &operator=(QlrpcApplication &) = delete;
};