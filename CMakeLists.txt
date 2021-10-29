cmake_minimum_required(VERSION 3.10)
project(HttpServer)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread)

add_executable(HttpServer main.cpp master_service.cpp master_service.h http_service.cpp http_service.h http_router.cpp http_router.h thread_pool.cpp thread_pool.h http_utils.cpp http_utils.h utils.cpp utils.h)
