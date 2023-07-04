
# 登录注册样例
set(SRC_LIST_USER userservice.cpp ../user.pb.cc)
add_executable(user_server ${SRC_LIST_USER})
target_link_libraries(user_server ql_rpc protobuf)



# 好友查询样例
set(SRC_LIST_FRIEND friendservice.cpp ../friend.pb.cc)
add_executable(friend_server ${SRC_LIST_FRIEND})
target_link_libraries(friend_server ql_rpc protobuf)