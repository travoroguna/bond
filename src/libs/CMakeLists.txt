cmake_minimum_required(VERSION 3.22)
project(libs)

set(CMAKE_CXX_STANDARD 23)


add_library(io SHARED io/io.cpp)
target_link_libraries(io bondlib)
set_target_properties(io PROPERTIES PUBLIC_HEADER "io/io.bd")


add_library(os SHARED os/os.cpp)
target_link_libraries(os bondlib)
set_target_properties(os PROPERTIES PUBLIC_HEADER "os/os.bd")


add_library(gc SHARED gc/gc.cpp)
target_link_libraries(gc bondlib)
set_target_properties(gc PROPERTIES PUBLIC_HEADER "gc/gc.bd")


add_library(isolate SHARED isolate/isolate.cpp)
target_link_libraries(isolate bondlib plibsys)
set_target_properties(isolate PROPERTIES PUBLIC_HEADER "isolate/isolate.bd")

add_library(reflection SHARED reflection/reflection.cpp)
target_link_libraries(reflection bondlib)
set_target_properties(reflection PROPERTIES PUBLIC_HEADER "reflection/reflection.bd")

add_library(socket SHARED socket/socket.cpp)
target_link_libraries(socket bondlib plibsys)
set_target_properties(socket PROPERTIES PUBLIC_HEADER "socket/socket.bd")


INSTALL(TARGETS io os gc isolate reflection socket
        LIBRARY DESTINATION libraries
        RUNTIME DESTINATION libraries
        PUBLIC_HEADER DESTINATION libraries)