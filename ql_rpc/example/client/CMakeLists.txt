#userservice client
# 登录注册样例client
set(SRC_LIST_USER cli_userservice.cpp ../user.pb.cc)
add_executable(user_client ${SRC_LIST_USER})
target_link_libraries(user_client ql_rpc protobuf)



#friendservice client
# 好友查询样例client
set(SRC_LIST_FRIEND cli_friendservice.cpp ../friend.pb.cc)
add_executable(friend_client ${SRC_LIST_FRIEND})
target_link_libraries(friend_client ql_rpc protobuf)