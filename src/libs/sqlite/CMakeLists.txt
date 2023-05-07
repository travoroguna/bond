cmake_minimum_required(VERSION 3.22)
project(sqlite)

set(CMAKE_CXX_STANDARD 23)


add_library(sqlite SHARED sqlite.cpp)
target_link_libraries(sqlite bondlib)
set_target_properties(sqlite PROPERTIES PUBLIC_HEADER "sqlite/sqlite.bd")
