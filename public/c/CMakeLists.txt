cmake_minimum_required(VERSION 2.8)

project(htidc_public)

#将lib库加入工程之中，相当于编译选项中的-I/htidc/public/lib
include_directories(../lib)

add_executable(a a.cpp)





