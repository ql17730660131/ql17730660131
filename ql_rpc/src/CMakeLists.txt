aux_source_directory(. SRC_LIST)
# set(SRC_LIST 
# 所有源文件名称
#     )
# add_library(ql_rpc STATIC ${SRC_LIST})

add_library(ql_rpc ${SRC_LIST})

#直接将protobuf链接进入静态库中，在example示例无需再进行链接protobuf
target_link_libraries(ql_rpc muduo_net muduo_base zookeeper_mt pthread)