#include "InetAddress.h"
#include <iostream>
int main()
{
    InetAddress addr("127.0.0.1",8080);
    std::cout << addr.toIpPort() << std::endl;
    std::cout << addr.toPort() << std::endl;
    std::cout << addr.toIp() << std::endl;
    return 0;
}