#!/bin/bash

set -e

# 如果没有build目录，创建该目录
if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*

cd `pwd`/build &&
    cmake .. &&
    make

# 回到项目根目录
cd ..

# 把头文件拷贝到 /usr/include/ql_muduo  so库拷贝到 /usr/lib    PATH
if [ ! -d /usr/include/ql_muduo ]; then 
    mkdir /usr/include/ql_muduo
fi


#for header in `ls src/include/*.h`
# for header in `ls *.h`
for header in `ls src/*.h`
do
    cp $header /usr/include/ql_muduo
done

cp `pwd`/lib/libql_muduo.so /usr/lib

echo "ql_muduo库安装完成..."

ldconfig