cmake_minimum_required(VERSION 3.22)
project(bond)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(src)


add_executable(bond src/main.cpp)
target_link_libraries(bond bondlib)

enable_testing()
add_subdirectory(tests)
