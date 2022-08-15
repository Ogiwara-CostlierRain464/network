cmake_minimum_required(VERSION 3.21)
project(network C)

set(CMAKE_C_STANDARD 11)

add_executable(network main.c)

file(GLOB_RECURSE LIB lib/*)
add_executable(epoll_sample
  ${LIB}
  epoll.c
)
