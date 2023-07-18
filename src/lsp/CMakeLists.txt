cmake_minimum_required(VERSION 3.22)
project(bondlsp)


set(BUILD_EXAMPLE OFF CACHE INTERNAL "")

if (LINUX)
set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -fexceptions -pthread -Wno-changes-meaning")
endif ()
add_executable(bondlsp common.hpp json.hpp json.hpp lsp.cpp lsp.hpp rpc.cpp rpc.hpp main.cpp)
target_link_libraries(bondlsp bond-lib)

install(TARGETS bondlsp)