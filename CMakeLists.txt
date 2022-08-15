cmake_minimum_required(VERSION 3.0)
project(network C)

set(CMAKE_C_STANDARD 11)

add_executable(network main.c)

file(GLOB_RECURSE LIB lib/*)
file(GLOB_RECURSE SILO silo/*)

add_executable(epoll_sample
  ${LIB}
  ${SILO}
  epoll.c
)
