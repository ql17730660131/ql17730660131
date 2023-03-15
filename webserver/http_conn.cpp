#include "http_conn.h"
#include <iostream>
#include <string.h>
// 定义HTTP响应的一些状态信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file from this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the requested file.\n";

// 网站的根目录
const char *qianlan_root = "/root/qianlan/code/resources";

// 所有的客户数
std::atomic<int> http_conn::m_user_count;
// 所有socket上的事件都被注册到同一个epoll内核事件中，所以设置成静态的
int http_conn::m_epollfd = -1;
// 设置文件描述非阻塞
void setnonblocking(int fd)
{
    int old_flag = fcntl(fd, F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);
}
// 添加文件描述符到epoll
void addfd(int epollfd, int fd, bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; // 对端异常断开底层处理EPOLLRDHUP
    // EpollErr和EpollHup会自动加上。
    if (one_shot)
    {
        // 防止同一个通信被不同的线程处理
        event.events |= EPOLLONESHOT;
    }

    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    if (!ret)
    {
        http_conn::m_user_count++; // 用户数加1
    }
    // 设置文件描述符非阻塞
    setnonblocking(fd);
}
// 重置socket上EPOLLONESHOT事件 确保下一次可读时，epollin时间能被触发
void reset_oneshot(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}
http_conn::http_conn(/* args */) : m_sockfd(-1), m_linger(false)
{
    init();
}

http_conn::~http_conn()
{
}
// 从epoll删除文件描述符
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    int ret = close(fd);
    if (!ret)
    {
        http_conn::m_user_count--; // 用户数-1
    }
    std::cout << "删除用户成功" << std::endl;
}
// 线程池处理函数
void http_conn::handle_read()
{
    bool ret = this->read();
    if (ret)
    {
        process();
    }
    else
    {
        close_conn();
    }
}
// 线程池处理函数写
void http_conn::handle_write()
{
    if (this->write()) // 一次性1把数据写完
    {
        // std::cout << "数据发送成功或一次没有写完，下次再写" << std::endl;
    }
    else
    {
        std::cout << "发送失败，关闭连接" << std::endl;
        close_conn();
    }
}
// 修改文件描述符
void modfd(int epollfd, int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

// 初始化新接收的连接
void http_conn::init(const int sockfd, const sockaddr_in &addr)
{
    m_sockfd = sockfd;
    m_address = addr; // 有函数
    // 设置端口复用
    int reuse = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    addfd(m_epollfd, sockfd, true); // 注意修改
    std::cout << "有新的连接成功" << std::endl;
}
// 初始化成员
void http_conn::init()
{
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_checked_index = 0;
    m_start_line = 0;
    m_read_index = 0;
    m_method = GET; // 请求方法
    m_url = NULL;   // 客户请求的目标文件的文件名
    m_version = NULL;
    m_host = NULL;
    m_content_length = 0;
    bzero(m_read_buf, READ_BUFFER_SIZE);
    bzero(m_write_buf, READ_BUFFER_SIZE);
    bzero(m_real_file, FILENAME_LEN);
    m_file_address = NULL; // 客户请求的目标文件被mmap到内存中的起始位置
    m_iv_count = 0;

    bytes_to_send = 0;   // 将要发送的数据的字节数
    bytes_have_send = 0; // 已经发送的字节数
}
// 关闭连接
void http_conn::close_conn()
{
    if (m_sockfd != -1)
    {
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        printf("删除客户端的IP地址: %s, 端口: %d\n",
               inet_ntoa(m_address.sin_addr),
               ntohs(m_address.sin_port));
        init();//初始化很关键
    }
}
// 非阻塞读
bool http_conn::read()
{ // 读到0关闭文件描述符
    // std::cout << "一次性读完数据" << std::endl;

    while (1)
    {
        int len = recv(m_sockfd, m_read_buf + m_read_index, sizeof(m_read_buf) - m_read_index, 0);
        if (len > 0)
        {
            m_read_index += len;
            continue;
        }
        else if (len == 0)
        {
            // 非阻塞模式下和阻塞模式是一样的 => 判断对方是否断开连接
            printf("客户端断开了连接...\n");
            // 将这个文件描述符从epoll模型中删除
            removefd(http_conn::m_epollfd, m_sockfd);
            return false;
        }
        else
        { // len == -1
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            {
                printf("数据读完了...\n");
                break;
            }
            perror("recv");
            return false;
        }
    }
    m_read_buf[m_read_index] = '\0';
    std::cout << "读取到的数据：" << m_read_buf << std::endl;
    // m_read_index = 0;//最后完成后置零
    return true;
}

// 有线程池的工作线程调用，处理HTTP请求的入口函数
void http_conn::process()
{
    // 解析HTTP请求
    // 解析HTTP请求
    HTTP_CODE read_ret = process_read();
    if (read_ret == NO_REQUEST)
    {
        modfd(m_epollfd, m_sockfd, EPOLLIN);
        return;
    }

    // 生成响应
    bool write_ret = process_write(read_ret);
    if (!write_ret)
    {
        close_conn();
    }
    modfd(m_epollfd, m_sockfd, EPOLLOUT);
}
// 主状态机 解析客户端请求
http_conn::HTTP_CODE http_conn::process_read()
{
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char *text = NULL;
    while ((m_check_state == CHECK_STATE_CONTENT) && (line_status == LINE_OK) || ((line_status = prase_line()) == LINE_OK))
    { // 解析到了一行完整的数据，或者解析到了请求体
        // 获取一行数据
        text = getline();

        m_start_line = m_checked_index;
        // std::cout << "get 1 http line: " << text << std::endl;
        switch (m_check_state)
        {
        case CHECK_STATE_REQUESTLINE:
        {
            ret = prase_request_line(text);
            if (ret == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            break;
        }
        case CHECK_STATE_HEADER:
        {
            ret = prase_headers(text);
            if (ret == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            else if (ret == GET_REQUEST)
            {
                return do_request();
            }
            break;
        }
        case CHECK_STATE_CONTENT:
        {
            ret = prase_content(text);
            if (ret == GET_REQUEST)
            {
                return do_request();
            }
            line_status = LINE_OPEN;
            break;
        }
        default:
        {
            return INTERNAL_ERROR;
        }
        }
    }
    return NO_REQUEST;
}
http_conn::HTTP_CODE http_conn::prase_request_line(char *text)
{
    std::cout << "get 1 http line: " << text << std::endl;
    // GET / HTTP/1.1
    m_url = strpbrk(text, " \t");
    *m_url++ = '\0';
    // GET\0/ HTTP/1.1
    char *method = text;
    if (strcasecmp(method, "GET") == 0)
    {
        m_method = GET;
    }
    else
    {
        return BAD_REQUEST;
    }
    // 此时m_url跳过了第一个 空格 或 '\t'，但不知道之后是否还有
    // 将m_url向后偏移，通过查找继续跳过 空格 和 '\t'，指向请求资源的第一个字符
    m_url += strspn(m_url, " \t");

    m_version = strpbrk(m_url, " \t");
    if (!m_version)
    {
        return BAD_REQUEST;
    }
    // GET\0/\0HTTP/1.1
    *m_version++ = '\0';
    m_version += strspn(m_version, " \t");

    // GET\0/http://192.168.2.1/index.html\0HTTP/1.1
    if (strncasecmp(m_url, "http://", 7) == 0)
    {
        m_url += 7;
        // 返回首次出现'/'的位置的指针
        m_url = strchr(m_url, '/');
    }

    if (strncasecmp(m_url, "https://", 8) == 0)
    {
        m_url += 8;
        // 返回首次出现'/'的位置的指针
        m_url = strchr(m_url, '/');
    }

    if (!m_url || m_url[0] != '/')
    {
        // HTTP请求报文有语法错误
        return BAD_REQUEST;
    }
    // 请求行处理完毕，将主状态机状态转移处理请求头
    m_check_state = CHECK_STATE_HEADER;
    // 请求不完整
    return NO_REQUEST;
}
http_conn::HTTP_CODE http_conn::prase_headers(char *text)
{
    // 解析空行
    if (text[0] == '\0')
    {
        if (m_content_length != 0)
        {
            // POST请求，主状态机跳转到消息体处理状态
            m_check_state = CHECK_STATE_CONTENT;
            // 请求不完整
            return NO_REQUEST;
        }
        // GET请求，获得了完整的HTTP请求
        return GET_REQUEST;
    }
    // 解析HTTP请求头各字段
    // Connection
    else if (strncasecmp(text, "Connection:", 11) == 0)
    {
        text += 11;
        // 跳过'\t'与空格
        text += strspn(text, " \t");
        if (strcasecmp(text, "keep-alive") == 0)
        {
            m_linger = true;
        }
    }
    // Content-length
    else if (strncasecmp(text, "Content-length:", 15) == 0)
    {
        text += 15;
        // 跳过'\t'与空格
        text += strspn(text, " \t");
        m_content_length = atol(text);
    }
    // Host
    else if (strncasecmp(text, "Host:", 5) == 0)
    {
        text += 5;
        // 跳过'\t'与空格
        text += strspn(text, " \t");
        m_host = text;
    }
    else
    {
        // std::cout << "Unknown Request Header Field:" << text << std::endl;
    }
    // 请求不完整
    return NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::prase_content(char *text)
{
    if (m_read_index >= (m_content_length + m_checked_index))
    {
        text[m_content_length] = '\0';
        // 获得了完整的HTTP请求
        return GET_REQUEST;
    }
    // 请求不完整
    return NO_REQUEST;
}
// 解析一行，根据\r\n
http_conn::LINE_STATUS http_conn::prase_line()
{
    char temp;
    for (; m_checked_index < m_read_index; ++m_checked_index)
    {
        temp = m_read_buf[m_checked_index];
        if (temp == '\r')
        {
            if ((m_checked_index + 1) == m_read_index)
                return LINE_OPEN; //// 读取的行不完整，需要继续接收
            else if (m_read_buf[m_checked_index + 1] == '\n')
            { // 将'\r\n'改为'\0\0'
                m_read_buf[m_checked_index++] = '\0';
                m_read_buf[m_checked_index++] = '\0';
                return LINE_OK; // 完整读取一行
            }
        }
        else if (temp == '\n')
        {
            if ((m_checked_index > 1) && (m_read_buf[m_checked_index - 1] == '\r'))
            { // 将'\r\n'改为'\0\0'
                m_read_buf[m_checked_index - 1] = '\0';
                m_read_buf[m_checked_index++] = '\0';
                return LINE_OK; // 完整读取一行
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}
http_conn::HTTP_CODE http_conn::do_request()
{
    // "/home/nowcoder/webserver/resources"
    strcpy(m_real_file, qianlan_root);
    int len = strlen(qianlan_root);
    // std::cout << "m_real_file:" << m_real_file << std::endl;
    strncpy(m_real_file + len, m_url, FILENAME_LEN - len - 1);
    // 获取m_real_file文件的相关的状态信息，-1失败，0成功
    if (stat(m_real_file, &m_file_stat) < 0)
    {
        return NO_RESOURCE;
    }

    // 判断访问权限
    if (!(m_file_stat.st_mode & S_IROTH))
    {
        return FORBIDDEN_REQUEST;
    }

    // 判断是否是目录
    if (S_ISDIR(m_file_stat.st_mode))
    {
        return BAD_REQUEST;
    }

    // 以只读方式打开文件
    int fd = open(m_real_file, O_RDONLY);
    // 创建内存映射
    m_file_address = (char *)mmap(0, m_file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    return FILE_REQUEST;
}

// 非阻塞写
bool http_conn::write()
{
    // 给已经关闭的socket写时，会发生EPOLLERR，关闭文件描述符

    // strcpy(m_write_buf, "abc");
    // int len = send(m_sockfd, m_read_buf, strlen(m_read_buf), 0);
    // reset_oneshot(http_conn::m_epollfd, m_sockfd);
    int temp = 0;

    if (bytes_to_send == 0)
    {
        // 将要发送的字节为0，这一次响应结束。
        reset_oneshot(m_epollfd, m_sockfd);
        // modfd(m_epollfd, m_sockfd, EPOLLIN);
        init();
        return true;
    }

    while (1)
    {
        // 分散写
        temp = writev(m_sockfd, m_iv, m_iv_count);
        if (temp <= -1)
        {
            // 如果TCP写缓冲没有空间，则等待下一轮EPOLLOUT事件，虽然在此期间，
            // 服务器无法立即接收到同一客户的下一个请求，但可以保证连接的完整性。
            if (errno == EAGAIN)
            {
                // std::cout << "/* EAGAIN */" << std::endl;
                modfd(m_epollfd, m_sockfd, EPOLLOUT);
                return true;
            }
            unmap();
            return false;
        }

        bytes_have_send += temp;
        bytes_to_send -= temp;
        // 第一个iovec的数据已发送完，发送第二个iovec的数据
        if (bytes_have_send >= m_iv[0].iov_len)
        {
            m_iv[0].iov_len = 0;
            m_iv[1].iov_base = m_file_address + (bytes_have_send - m_write_index);
            m_iv[1].iov_len = bytes_to_send;
        }
        else
        {
            m_iv[0].iov_base = m_write_buf + bytes_have_send;
            m_iv[0].iov_len = m_iv[0].iov_len - temp;
        }

        if (bytes_to_send <= 0)
        {
            std::cout << "发送完成" << std::endl;
            // 没有数据要发送了
            unmap();
            reset_oneshot(m_epollfd, m_sockfd);
            // modfd(m_epollfd, m_sockfd, EPOLLIN);

            if (m_linger)
            {
                std::cout << "长连接" << std::endl;
                init();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

// 往写缓冲中写入待发送的数据
bool http_conn::add_response(const char *format, ...)
{
    if (m_write_index >= WRITE_BUFFER_SIZE)
    {
        return false;
    }
    va_list arg_list;
    va_start(arg_list, format);
    int len = vsnprintf(m_write_buf + m_write_index, WRITE_BUFFER_SIZE - 1 - m_write_index, format, arg_list);
    if (len >= (WRITE_BUFFER_SIZE - 1 - m_write_index))
    {
        return false;
    }
    m_write_index += len;
    va_end(arg_list);
    return true;
}

bool http_conn::add_status_line(int status, const char *title)
{
    return add_response("%s %d %s\r\n", "HTTP/1.1", status, title);
}

bool http_conn::add_headers(int content_len)
{
    add_content_length(content_len);
    add_content_type();
    add_linger();
    add_blank_line();
    return true;
}

bool http_conn::add_content_length(int content_len)
{
    return add_response("Content-Length: %d\r\n", content_len);
}

bool http_conn::add_linger()
{
    return add_response("Connection: %s\r\n", (m_linger == true) ? "keep-alive" : "close");
}

bool http_conn::add_blank_line()
{
    return add_response("%s", "\r\n");
}

bool http_conn::add_content(const char *content)
{
    return add_response("%s", content);
}

bool http_conn::add_content_type()
{
    return add_response("Content-Type:%s\r\n", "text/html");
}

// 根据服务器处理HTTP请求的结果，决定返回给客户端的内容
bool http_conn::process_write(HTTP_CODE ret)
{
    switch (ret)
    {
    case INTERNAL_ERROR:
    {
        add_status_line(500, error_500_title);
        add_headers(strlen(error_500_form));
        if (!add_content(error_500_form))
        {
            return false;
        }
        break;
    }
    case BAD_REQUEST:
    {
        add_status_line(400, error_400_title);
        add_headers(strlen(error_400_form));
        if (!add_content(error_400_form))
        {
            return false;
        }
        break;
    }
    case NO_RESOURCE:
    {
        add_status_line(404, error_404_title);
        add_headers(strlen(error_404_form));
        if (!add_content(error_404_form))
        {
            return false;
        }
        break;
    }
    case FORBIDDEN_REQUEST:
    {
        add_status_line(403, error_403_title);
        add_headers(strlen(error_403_form));
        if (!add_content(error_403_form))
        {
            return false;
        }
        break;
    }
    case FILE_REQUEST:
    {
        add_status_line(200, ok_200_title);
        if (m_file_stat.st_size != 0)
        {
            add_headers(m_file_stat.st_size);
            m_iv[0].iov_base = m_write_buf;
            m_iv[0].iov_len = m_write_index;
            m_iv[1].iov_base = m_file_address;
            m_iv[1].iov_len = m_file_stat.st_size;
            m_iv_count = 2;
            bytes_to_send = m_write_index + m_file_stat.st_size;
            return true;
        }
        else
        {
            const char *ok_string = "<html><body></body></html>";
            // 消息报头+空行
            add_headers(strlen(ok_string));
            // 响应正文
            if (!add_content(ok_string))
                return false;
        }
    }
    default:
        return false;
    }
    // 除FILE_REQUEST状态外，其余状态只申请一个iovec，指向响应报文缓冲区
    m_iv[0].iov_base = m_write_buf;
    m_iv[0].iov_len = m_write_index;
    m_iv_count = 1;
    bytes_to_send = m_write_index;
    return true;
}
// 对内存映射区执行munmap操作
void http_conn::unmap()
{
    if (m_file_address)
    {
        munmap(m_file_address, m_file_stat.st_size);
        m_file_address = 0;
    }
}