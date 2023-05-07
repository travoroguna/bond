cmake_minimum_required(VERSION 3.22)
project(bond-fmt)

set(CMAKE_CXX_STANDARD 23)


add_library(bond-fmt SHARED fmt.cpp fmt.h)
target_link_libraries(bond-fmt bondlib)
set_target_properties(bond-fmt PROPERTIES PUBLIC_HEADER "fmt.bd")
set_target_properties(bond-fmt PROPERTIES OUTPUT_NAME "fmt")

INSTALL(TARGETS bond-fmt
        LIBRARY DESTINATION libraries
        RUNTIME DESTINATION libraries
        PUBLIC_HEADER DESTINATION libraries)