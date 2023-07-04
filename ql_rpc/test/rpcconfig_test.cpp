#include"ql_rpcconfig.h"

int main()
{
    RpcConfig rpc_Config;
    std::string file_name = "../bin/test.conf";
    rpc_Config.LoadConfigFile(file_name.c_str());

}