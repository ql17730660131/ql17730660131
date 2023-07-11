

# 定义参与编译的源代码文件 
aux_source_directory(. SRC_LIST)
# 编译生成动态库ql_muduo
add_library(ql_muduo SHARED ${SRC_LIST})

target_link_libraries(ql_muduo pthread) 


