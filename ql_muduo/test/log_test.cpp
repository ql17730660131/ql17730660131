//加上MUDEBUG会打印debug
#define MUDEBUG
#include <Logger.h>

int main()
{
    LOG_INFO("qianlan...%d %s", 6, "llll");
    LOG_DEBUG("qianlan...%d %s", 6, "llll");
    LOG_ERROR("qianlan...%d %s", 6, "llll");
    LOG_FATAL("qianlan...%d %s", 6, "llll");
}