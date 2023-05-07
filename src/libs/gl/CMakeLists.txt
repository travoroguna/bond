cmake_minimum_required(VERSION 3.22)
project(gl)

set(CMAKE_CXX_STANDARD 23)

CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.8
)

#CPMAddPackage(
#        NAME glew
#        GITHUB_REPOSITORY nigels-com/glew
#        GIT_TAG glew-2.2.0
#)
#
#if (glew_ADDED)
#
#    add_subdirectory(${glew_SOURCE_DIR}/build/cmake ${glew_BINARY_DIR}  )
#endif ()

add_subdirectory(glad)

add_library(imgui SHARED imgui/imguimodule.cpp imgui/imconfig.h imgui/imgui.cpp imgui/imgui.h
        imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_glfw.h imgui/imgui_impl_opengl3.cpp imgui/imgui_impl_opengl3.h imgui/imgui_impl_opengl3_loader.h
        imgui/imgui_internal.h imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imstb_rectpack.h
        imgui/imstb_textedit.h imgui/imstb_truetype.h imgui/imgui_stdlib.cpp imgui/imgui_stdlib.h
        )


target_link_libraries(imgui bondlib glad glfw)
set_target_properties(imgui PROPERTIES PUBLIC_HEADER "imgui/imgui.bd")


add_library(gl SHARED gl/glmodule.cpp gl/glmodule.h)

target_link_libraries(gl bondlib glad glfw)
set_target_properties(imgui PROPERTIES PUBLIC_HEADER "gl/gl.bd")


INSTALL(TARGETS imgui gl
        LIBRARY DESTINATION libraries/gl
        RUNTIME DESTINATION libraries/gl
        PUBLIC_HEADER DESTINATION libraries/gl)