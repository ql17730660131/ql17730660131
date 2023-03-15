#ifndef HTTP_CONN_H
#define HTTP_CONN_H
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/uio.h>
#include <atomic>

// 添加文件描述符到epoll
extern void addfd(int epollfd, int fd, bool one_shot);
// 从epoll删除文件描述符
extern void removefd(int epollfd, int fd);
// 修改文件描述符
extern void modfd(int epollfd, int fd, int ev);
// 重置oneshot
void reset_oneshot(int epollfd, int fd);
class http_conn
{
public:
    static int m_epollfd;    // 所有socket上的时间都被注册到同一个epoll
    static std::atomic<int> m_user_count; // 统计用户数量
    // static const int 定义时初始化，直接使用
    static const int FILENAME_LEN = 200;       // 文件名的最大长度
    static const int READ_BUFFER_SIZE = 4096;  // 读缓冲区的大小
    static const int WRITE_BUFFER_SIZE = 2048; // 写缓冲区的大小
    // HTTP请求方法，这里只支持GET
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT
    };

    /*
        解析客户端请求时，主状态机的状态
        CHECK_STATE_REQUESTLINE:当前正在分析请求行
        CHECK_STATE_HEADER:当前正在分析头部字段
        CHECK_STATE_CONTENT:当前正在解析请求体
    */
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };

    /*
        服务器处理HTTP请求的可能结果，报文解析的结果
        NO_REQUEST          :   请求不完整，需要继续读取客户数据
        GET_REQUEST         :   表示获得了一个完成的客户请求
        BAD_REQUEST         :   表示客户请求语法错误
        NO_RESOURCE         :   表示服务器没有资源
        FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
        FILE_REQUEST        :   文件请求,获取文件成功
        INTERNAL_ERROR      :   表示服务器内部错误
        CLOSED_CONNECTION   :   表示客户端已经关闭连接了
    */
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    // 从状态机的三种可能状态，即行的读取状态，分别表示
    // 1.读取到一个完整的行 2.行出错 3.行数据尚且不完整
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

private:
    /* data */
    int m_sockfd;          // 该http连接的socket文件描述符
    sockaddr_in m_address; // 通信的socket地址
    char m_read_buf[READ_BUFFER_SIZE];
    int m_read_index; // 在构造函数初始化
    char m_write_buf[WRITE_BUFFER_SIZE];
    int m_write_index; // 在构造函数初始化

public:
    http_conn(/* args */);
    ~http_conn();
    // 处理客户请求
    void process();
    void handle_read();//线程池处理函数
    void handle_write();//线程池处理函数
    // 初始化新接收的连接
    void init(const int sockfd, const sockaddr_in &addr);
    // 初始化成员
    void init();
    // 关闭连接
    void close_conn();

    bool read();  // 非阻塞读
    bool write(); // 非阻塞写

    // 往写缓冲中写入待发送的数据
    bool add_response(const char *format, ...);
    // 根据服务器处理HTTP请求的结果，决定返回给客户端的内容
    bool process_write(HTTP_CODE ret);

    // 这一组函数被process_write调用以填充HTTP应答。
    void unmap();
    bool add_content(const char *content);
    bool add_content_type();
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

private:
    // 主状态机 解析客户端请求
    HTTP_CODE process_read();
    HTTP_CODE prase_request_line(char *text); // 解析请求首行
    HTTP_CODE prase_headers(char *text);      // 解析请求头
    HTTP_CODE prase_content(char *text);      // 解析请求体
    char *getline() { return m_read_buf + m_start_line; }
    LINE_STATUS prase_line();  // 解析一行，根据\r\n
    HTTP_CODE do_request();    //
    int m_checked_index = 0;   // 当前分析的字符在读缓冲区的位置
    int m_start_line;          // 当前解析的行的起始位置
    CHECK_STATE m_check_state; // 主状态机目前的状态

    METHOD m_method;                // 请求方法
    char m_real_file[FILENAME_LEN]; // 客户请求的目标文件的完整路径，其内容等于 doc_root + m_url, doc_root是网站根目录
    char *m_url;                    // 客户请求的目标文件的文件名
    char *m_version;                // HTTP协议版本号，我们仅支持HTTP1.1
    char *m_host;                   // 主机名
    int m_content_length;           // HTTP请求的消息总长度
    bool m_linger;                  // HTTP请求是否要求保持连接

    char *m_file_address;    // 客户请求的目标文件被mmap到内存中的起始位置
    struct stat m_file_stat; // 目标文件的状态。通过它我们可以判断文件是否存在、是否为目录、是否可读，并获取文件大小等信息
    struct iovec m_iv[2];    // 我们将采用writev来执行写操作，所以定义下面两个成员，其中m_iv_count表示被写内存块的数量。
    int m_iv_count;

    int bytes_to_send;   // 将要发送的数据的字节数
    int bytes_have_send; // 已经发送的字节数
};

#endif