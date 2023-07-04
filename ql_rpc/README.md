项目使用的protobuf版本为3.21.6
使用zookeeper进行服务部署 版本zookeeper-3.4.10

过程
通过读配置文件，存入zookeeper的ip port 
客户端通过zookeeper查询rpc服务的ip与port
通过rpc服务的ip与port进行rpc通信
此过程在ql_rpcchannel.cpp中实现


安装

1直接将项目下载后解压
2可以自行进行编译（先删除build与文件夹下内容，进入cd build/ cmake .. make）
也可以直接使用（libql_rpc.a已经编译好）

3使用
先启动zookeeper服务，等待进行连接 服务注册
修改配置文件ql_rpc.conf,客户端只需修改zk的ip和port(后两项)
服务端：
包含  ql_rpcapplication.h  rpcprovider.h

调用框架的初始化操作
QlrpcApplication::Init(argc, argv);
argc, argv为main函数传入参数，在执行时格式如下
command -i <config_filename>


// provider是一个rpc网络服务对象。把写好Service对象发布到rpc节点上
参考example/server/userservice.cpp
RpcProvider provider;
provider.NotifyService(new Service());

// 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
provider.Run();


客户端：
包含ql_rpcapplication.h  ql_rpcchannel.h
同样需要配置文件，格式如上
服务构造需要QlrpcChannel指针
如：
// 演示调用远程发布的rpc方法Login
    user::UserServiceRpc_Stub stub(new QlrpcChannel());
详细参考example/client/cli_userservice.cpp

// 发起rpc方法的调用  同步的rpc调用过程
stub.Login(nullptr, &request, &response, nullptr); 