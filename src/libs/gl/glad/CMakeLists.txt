cmake_minimum_required(VERSION 3.22)
project(glad)

set(CMAKE_CXX_STANDARD 23)


add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})