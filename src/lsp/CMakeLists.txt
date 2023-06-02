cmake_minimum_required(VERSION 3.22)
project(bondlsp)


set(BUILD_EXAMPLE OFF CACHE INTERNAL "")

add_executable(bondlsp common.hpp json.hpp json.hpp lsp.cpp lsp.hpp rpc.cpp rpc.hpp main.cpp)
target_link_libraries(bondlsp bondlib)

install(TARGETS bondlsp)