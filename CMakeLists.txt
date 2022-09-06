cmake_minimum_required(VERSION 3.0)
project(network C)

set(CMAKE_C_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(network main.c)

file(GLOB_RECURSE LIB lib/*)
file(GLOB_RECURSE SILO silo/*)

add_executable(epoll_sample
  ${LIB}
  ${SILO}
  epoll_sample.c
)

add_executable(silo_sample
  ${LIB}
  ${SILO}
  silo_sample.c
)
target_link_libraries(silo_sample PUBLIC "-pthread")


add_executable(silo_epoll
  ${LIB}
  ${SILO}
  silo_epoll.c
  )
target_link_libraries(silo_epoll PUBLIC "-pthread")

add_executable(client
  ${LIB}
  client.c
)
target_link_libraries(client PUBLIC "-pthread")


enable_testing()

add_executable(unit_test
  ${LIB}
  ${SILO}
  test/unit.c
)
add_test(
  NAME unit_test
  COMMAND $<TARGET_FILE:unit_test> --only==a
)