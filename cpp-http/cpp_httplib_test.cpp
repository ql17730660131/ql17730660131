#include <httplib.h>
#include <sys/mman.h>
using namespace httplib;
std::string process(std::string str); // 通过后缀返回对应类型
int main()
{

        Server svr;
        // svr.set_base_dir("/root/qianlan/test/dir2/cpp-http/resources/");
        svr.set_mount_point("/qian", "./resources");
        // svr.set_base_dir()
        svr.Get("/", [](const Request &req, Response &res)
                {
                        struct stat file_stat;
                        std::string real_file = "/root/qianlan/test/dir2/cpp-http/index.html";

                        if(stat(real_file.c_str(), &file_stat) < 0)
                        {
                                std::cout<<""<<std::endl;
                        }
                        int size = file_stat.st_size;//文件大小
                        res.status = 200;
                        res.version = "HTTP/1.1";
                        // 以只读方式打开文件
                        int fd = open(real_file.c_str(), O_RDONLY);
                        // 创建内存映射
                        char *file_address = (char *)mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                        close(fd);
                        std::string str_size = std::to_string(size);
                        std::cout<<"length: "<<size<<std::endl;
                        res.set_content_provider(size,"index.html",
                        [file_address](size_t offset, size_t length, DataSink &sink) {
                                        size_t DATA_CHUNK_SIZE = 4;
                                        
                                        auto out_len = 4096;
                                                // std::min(static_cast<size_t>(length), DATA_CHUNK_SIZE);
                                        auto ret =
                                                sink.write(file_address+offset, out_len);
                                        // std::cout<<"length"<<length<<std::endl;
                                        // std::cout<<"写了"<<out_len<<"字节"<<std::endl;
                                        return true;
                        },[&](bool success) {
                                if (file_address)
                                {
                                        munmap(file_address, file_stat.st_size);
                                        file_address = 0;
                                        // std::cout<<"关闭内存映射"<<std::endl;
                                }
                                close(fd);
                                // std::cout<<"关闭fd"<<std::endl;
                        }
                        ); });

        svr.Get("/str", [](const Request &req, Response &res)
                { 
                        auto data = new std::string("abcdefgfsegfewfwssb");
                        res.set_content_provider(
                                data->size(), "text/plain",
                                [data](size_t offset, size_t length, DataSink &sink) {
                                        size_t DATA_CHUNK_SIZE = 4;
                                        const auto &d = *data;
                                        auto out_len =
                                                std::min(static_cast<size_t>(length), DATA_CHUNK_SIZE);
                                        auto ret =
                                                sink.write(&d[static_cast<size_t>(offset)], out_len);
                                        // std::cout<<"length"<<length<<std::endl;
                                        // std::cout<<"写了"<<out_len<<"字节"<<std::endl;
                                        return true;
                        },
                        [data](bool success) {
                     
                        delete data;
                   }); });
        svr.Get("/hi", [](const Request &req, Response &res)
                {
                        // res.status = 200;
                        // res.version = "HTTP/1.1";
                        std::cout <<"res.status: "<< res.status << std::endl;

                        std::cout <<"res.version: "<< res.version << std::endl;
                        std::cout << req.body << std::endl;
                        std::cout << req.path << std::endl;
                        res.set_content("Hello World!", "text/html"); });

        svr.Get("/qianlan", [](const Request &req, Response &res)
                { 
                        std::cout <<"content_length_: "<< req.content_length_ << std::endl;
                        std::cout << req.body << std::endl;
                        std::cout << req.path << std::endl;
                        std::cout << req.version << std::endl;
                        res.set_content("Hello qianlan...", "text/html"); }); // 最后一个参数是正文类型
        
        svr.Get("/bd", [](const Request &req, Response &res)
                {
                        
                        const char *BaiDuURL = "https://www.baidu.com/";// 重定向到百度
                        res.set_redirect(BaiDuURL); });
        // 正则匹配用法/numbers/后面加任意数字
        svr.Get(R"(/numbers/(\d+))", [&](const Request &req, Response &res)
                {
          auto numbers = req.matches[1];
          res.set_content(numbers, "text/plain"); });

        svr.Get(R"(/resources/(.+))", [&](const Request &req, Response &res)
                {
                        std::cout << req.path << std::endl;
                        auto str = req.matches[1].str();

                        struct stat file_stat;
                        std::cout << "str: " << str << std::endl;
                        std::string real_file = std::string("/root/qianlan/test/dir2/cpp-http/resources/") + str;


                        std::cout<<"文件路径："<< real_file <<std::endl;


                        if(stat(real_file.c_str(), &file_stat) < 0)
                        {
                                std::cout<<"获取文件信息失败"<<std::endl;
                        }
                        int size = file_stat.st_size;//文件大小
                        res.status = 200;
                        res.version = "HTTP/1.1";
                        // 以只读方式打开文件
                        int fd = open(real_file.c_str(), O_RDONLY);
                        // 创建内存映射
                        char *file_address = (char *)mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                        close(fd);
                        std::string str_size = std::to_string(size);
                        std::cout<<"length: "<<size<<std::endl;
                        res.set_header("Content-Length",str_size);


                        //获取后缀
                        std::size_t index = str.find_last_of('.');
                        
                        std::string str_suf = str.substr(index+1,str.size()-index-1);
                        std::cout << "str_suf: " << str_suf << std::endl;

                        res.set_content_provider(size,process(str_suf),
                        [file_address](size_t offset, size_t length, DataSink &sink) {
                                        size_t DATA_CHUNK_SIZE = 4;

                                        auto out_len = 4096;
                                                // std::min(static_cast<size_t>(length), DATA_CHUNK_SIZE);
                                        auto ret =
                                                sink.write(file_address+offset, out_len);
                                        // std::cout<<"length"<<length<<std::endl;
                                        // std::cout<<"写了"<<out_len<<"字节"<<std::endl;
                                        return true;
                        },[&](bool success) {
                                if (file_address)
                                {
                                        munmap(file_address, file_stat.st_size);
                                        file_address = 0;
                                        // std::cout<<"关闭内存映射"<<std::endl;
                                }
                                close(fd);
                                // std::cout<<"关闭fd"<<std::endl;
                        }
                        ); });

        // svr.Get("/body-header-param", [](const Request &req, Response &res)
        //         {
        //   if (req.has_header("Content-Length")) {
        //     auto val = req.get_header_value("Content-Length");
        //     std::cout << val << std::endl;
        //   }
        //   if (req.has_param("key")) {
        //     auto val = req.get_param_value("key");
        //     std::cout << val << std::endl;
        //   }
        //   res.set_content(req.body, "text/plain"); });

        // svr.Get("/stop", [&](const Request &req, Response &res)
        //         { svr.stop(); });

        svr.listen("0.0.0.0", 9000);
        // int port = svr.bind_to_any_port("0.0.0.0");
        // svr.listen_after_bind();
}

// struct stat m_file_stat;
//                         std::string m_real_file = "/root/qianlan/test/dir2/cpp-http/1.PNG";

//                         if(stat(m_real_file.c_str(), &m_file_stat) < 0)
//                         {
//                                 std::cout<<""<<std::endl;
//                         }
//                         int size = m_file_stat.st_size;//文件大小
//                         res.status = 200;
//                         res.version = "HTTP/1.1";

//                         std::string str_size = std::to_string(size);
//                         DataSink ds;
//                         ds.write = handle_write;//可能有问题
//                         res.set_header("Content-Length",str_size);
//                         res.set_content_provider(size,"image/png",ConPvder);

// // 对内存映射区执行munmap操作
// void http_conn::unmap()
// {
//     if (m_file_address)
//     {
//         munmap(m_file_address, m_file_stat.st_size);
//         m_file_address = 0;
//     }
// }

std::string process(std::string str) // 通过后缀返回对应类型
{
        if (str == std::string("html"))
        {
                return "text/html";
        }
        else if (str == std::string("png") || str == std::string("PNG"))
        {
                return "image/png";
        }
        else if (str == std::string("jpg"))
        {
                return "image/jpeg";
        }
        return {};
}

// 下面是下载文件的写法
//  svr.Get("/", [](const Request &req, Response &res)
//                  {
//                          struct stat file_stat;
//                          std::string real_file = "/root/qianlan/test/dir2/cpp-http/1.png";

//                         if(stat(real_file.c_str(), &file_stat) < 0)
//                         {
//                                 std::cout<<""<<std::endl;
//                         }
//                         int size = file_stat.st_size;//文件大小
//                         res.status = 200;
//                         res.version = "HTTP/1.1";
//                         // 以只读方式打开文件
//                         int fd = open(real_file.c_str(), O_RDONLY);
//                         // 创建内存映射
//                         char *file_address = (char *)mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
//                         close(fd);
//                         std::string str_size = std::to_string(size);
//                         std::cout<<"length: "<<size<<std::endl;
//                         res.set_header("Content-Length",str_size);
//                         res.set_header("Content-Disposition","attachment;   filename=1.png");

//                         res.set_content_provider(size,"application/octet-stream",
//                         [file_address](size_t offset, size_t length, DataSink &sink) {
//                                         size_t DATA_CHUNK_SIZE = 4;

//                                         auto out_len = 4096;
//                                                 // std::min(static_cast<size_t>(length), DATA_CHUNK_SIZE);
//                                         auto ret =
//                                                 sink.write(file_address+offset, out_len);
//                                         // std::cout<<"length"<<length<<std::endl;
//                                         // std::cout<<"写了"<<out_len<<"字节"<<std::endl;
//                                         return true;
//                         },[&](bool success) {
//                                 if (file_address)
//                                 {
//                                         munmap(file_address, file_stat.st_size);
//                                         file_address = 0;
//                                         // std::cout<<"关闭内存映射"<<std::endl;
//                                 }
//                                 close(fd);
//                                 // std::cout<<"关闭fd"<<std::endl;
//                         }
//                         ); });