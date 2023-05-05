cmake_minimum_required(VERSION 2.8.4)
project(epoll_echo)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_COMPILER g++)
find_library(IBVERBS_LIBRARY ibverbs)
find_library(RDMACM_LIBRARY rdmacm)

link_libraries(pthread ${IBVERBS_LIBRARY} ${RDMACM_LIBRARY})

add_executable(server Server.cpp Epoll.cpp)
add_executable(client Client.cpp Epoll.cpp)

