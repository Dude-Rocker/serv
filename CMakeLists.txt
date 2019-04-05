cmake_minimum_required(VERSION 3.1)

project(test_rpoject)


set(NAME_SRV "udp_com")

set(SRCS_SRV main.cpp udp_com.cpp)

find_package(Boost 1.65.0 COMPONENTS system regex filesystem REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I/home/gabi/Downloads/asio-1.11.0/include -pthread")

add_executable(${NAME_SRV} ${SRCS_SRV})

target_link_libraries(${NAME_SRV} ${Boost_LIBRARIES})
