cmake_minimum_required(VERSION 3.22)
project(ray-lib)

set(CMAKE_CXX_STANDARD 23)

CPMAddPackage(
        NAME raylib
        GITHUB_REPOSITORY raysan5/raylib
        GIT_TAG 4.5.0
        EXCLUDE_FROM_ALL YES
        GIT_SHALLOW ON
)

CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        GIT_TAG v0.8.2
        GIT_SHALLOW ON
)

add_library(ray-lib SHARED raymodule.cpp raymodule.h ray_impl.cpp
        raycore/raycore.h raycore/raycore.cpp raytexture/raytexture.h raytexture/raytexture_impl.cpp
        rayshapes/rayshapes.h rayshapes/rayshapes_impl.cpp raytext/raytext.h raytext/raytext_impl.cpp raytext/raytext.cpp)

target_link_libraries(ray-lib bondlib raylib_static magic_enum::magic_enum)
set_target_properties(ray-lib PROPERTIES PUBLIC_HEADER "raylib.bd")
set_target_properties(ray-lib PROPERTIES OUTPUT_NAME "raylib")


INSTALL(TARGETS ray-lib
        LIBRARY DESTINATION libraries/raylib
        RUNTIME DESTINATION libraries/raylib
        PUBLIC_HEADER DESTINATION libraries/raylib)