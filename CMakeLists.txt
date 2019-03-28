cmake_minimum_required(VERSION 3.1)

project(test_rpoject)


set(NAME_SRV "server")

set(SRCS_SRV main.cpp udp_asyn.cpp)

find_package(Boost 1.69.0 COMPONENTS system regex filesystem REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I/home/gabi/Downloads/asio-1.11.0/include -pthread")

add_executable(${NAME_SRV} ${SRCS_SRV})

target_link_libraries(${NAME_SRV} ${Boost_LIBRARIES})

set(NAME_CLN "client")
set(SRCS_CLN client.cpp)
add_executable(${NAME_CLN} ${SRCS_CLN})
target_link_libraries(${NAME_CLN} ${Boost_LIBRARIES})

set(NAME_CLN2 "client2")
set(SRCS_CLN2 client2.cpp)
add_executable(${NAME_CLN2} ${SRCS_CLN2})
target_link_libraries(${NAME_CLN2} ${Boost_LIBRARIES})
