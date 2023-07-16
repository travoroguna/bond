cmake_minimum_required(VERSION 3.22)
project(libs)

set(CMAKE_CXX_STANDARD 23)
option(BUILD_LIBS "Build shared libraries" ON)


if (BUILD_LIBS MATCHES ON)
    add_subdirectory(fmt)


#    add_library(io SHARED io/io.cpp)
#    target_link_libraries(io bondlib)
#    set_target_properties(io PROPERTIES PUBLIC_HEADER "io/io.bd")
#
#
#    add_library(os SHARED os/os.cpp)
#    target_link_libraries(os bondlib)
#    set_target_properties(os PROPERTIES PUBLIC_HEADER "os/os.bd")
#
#
#    add_library(socket SHARED socket/socket.cpp)
#    target_link_libraries(socket bondlib plibsysstatic)
#    set_target_properties(socket PROPERTIES PUBLIC_HEADER "socket/socket.bd")
#
#
#    INSTALL(TARGETS io os socket
#            LIBRARY DESTINATION libraries
#            RUNTIME DESTINATION libraries
#            PUBLIC_HEADER DESTINATION libraries)

endif ()