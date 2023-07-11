# 时间类测试
set(SRC_LIST_time_test Timestamp_test.cpp ../src/Timestamp.cpp)
add_executable(time_test ${SRC_LIST_time_test})
# target_link_libraries(time_test 库名字)

# 日志类测试
set(SRC_LIST_log_test log_test.cpp ../src/Logger.cpp ../src/Timestamp.cpp)
add_executable(log_test ${SRC_LIST_log_test})

# 地址类测试
set(SRC_LIST_addr_test Inet_addr_test.cpp ../src/InetAddress.cpp)
add_executable(addr_test ${SRC_LIST_addr_test})
