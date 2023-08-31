cmake_minimum_required(VERSION 3.22)
project(libs)

set(CMAKE_CXX_STANDARD 23)
option(BUILD_LIBS "Build shared libraries" ON)


if (BUILD_LIBS MATCHES ON)
    CPMAddPackage(
            NAME libuv
            GIT_REPOSITORY "https://github.com/libuv/libuv.git"
            GIT_TAG v1.46.0
            EXCLUDE_FROM_ALL YES
    )

    set(LIBUV_BUILD_SHARED OFF CACHE INTERNAL "")

    add_library(fs SHARED fs/fs.cpp)
    target_link_libraries(fs bond-lib)

    add_library(io SHARED io/io.cpp io/io.h)
    target_link_libraries(io bond-lib uv_a)


    INSTALL(TARGETS fs io
            LIBRARY DESTINATION libraries
            RUNTIME DESTINATION libraries
            PUBLIC_HEADER DESTINATION libraries)

endif ()