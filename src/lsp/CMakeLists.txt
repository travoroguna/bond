cmake_minimum_required(VERSION 3.22)
project(bondlsp)


set(BUILD_EXAMPLE OFF CACHE INTERNAL "")


add_library(bondlsp lsp.h lsp.cpp jsonrpcpp.hpp)
target_link_libraries(bondlsp bondlib)