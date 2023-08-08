cmake_minimum_required(VERSION 3.22)
project(bnk)

set(CMAKE_CXX_STANDARD 23)
set(SOURCES
        bnk.cpp bnk.h nuklear.h
        nuklear_glfw_gl3.h
        glad/include/glad/glad.h
        glad/include/KHR/khrplatform.h
        glad/src/glad.c
        )



CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.8
        EXCLUDE_FROM_ALL YES
)

add_library(bnk STATIC ${SOURCES})
target_include_directories(bnk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bnk PRIVATE glfw fmt::fmt-header-only gc gccpp)