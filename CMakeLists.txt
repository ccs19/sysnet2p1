cmake_minimum_required(VERSION 2.8.4)
project(sysnet2p1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
set(SOURCE_FILES TCPserver.c)
add_executable(sysnet2p1 ${SOURCE_FILES})