cmake_minimum_required(VERSION 3.22)
project(tests)

set(CMAKE_CXX_STANDARD 23)


add_executable(gc_test testgc.cpp test.h)
target_link_libraries(gc_test bondlib fmt::fmt)

add_executable(map_test testmap.cpp test.h)
target_link_libraries(map_test bondlib fmt::fmt)


add_test(gc gc_test)
add_test(map map_test)


