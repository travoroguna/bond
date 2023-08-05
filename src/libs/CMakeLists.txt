cmake_minimum_required(VERSION 3.22)
project(libs)

set(CMAKE_CXX_STANDARD 23)
option(BUILD_LIBS "Build shared libraries" ON)


if (BUILD_LIBS MATCHES ON)
    add_library(fs SHARED fs/fs.cpp)
    target_link_libraries(fs bond-lib)


    INSTALL(TARGETS fs
            LIBRARY DESTINATION libraries
            RUNTIME DESTINATION libraries
            PUBLIC_HEADER DESTINATION libraries)

endif ()