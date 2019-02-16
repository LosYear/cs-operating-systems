cmake_minimum_required(VERSION 3.6)
project(ex1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_STANDARD 14)

add_executable(ex1 main.cpp matrix.h matrix.cpp)