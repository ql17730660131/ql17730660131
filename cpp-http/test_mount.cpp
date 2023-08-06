#include <httplib.h>
#include <sys/mman.h>
#include <iostream>
using namespace httplib;
int main()
{
    Server svr;
    svr.set_mount_point("/", "./resources");
    // svr.set_base_dir("./");
    svr.set_file_request_handler([](const Request &req, Response &res)
                                 { 
                                    
                                    std::cout << "Request path: " << req.path << std::endl;
                                    // std::cout << "Response reason: " << res.reason << std::endl;
                                    
                                    std::cout << "Content-Type: " << res.get_header_value("Content-Type") << std::endl;
                                    std::cout << "Content-Type: " << res.get_header_value("Content-Type") << std::endl; });

    svr.set_pre_routing_handler([](const auto &req, auto &res)
                                {
                                    if (req.path == "/hello") 
                                    {
                                        res.set_content("world", "text/html");
                                        return Server::HandlerResponse::Handled;
                                    }
                                    return Server::HandlerResponse::Unhandled; });
    svr.listen("0.0.0.0", 9000);
}

// auto ret = svr.set_mount_point("/", "./www");
// if (!ret)
// {
//     std::cout << "The specified base directory doesn't exist..." << std::endl;
// }
