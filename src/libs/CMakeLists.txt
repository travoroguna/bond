cmake_minimum_required(VERSION 3.22)
project(libs)

set(CMAKE_CXX_STANDARD 23)


add_library(io SHARED io.cpp)
target_link_libraries(io bondlib)
set_target_properties(io PROPERTIES PUBLIC_HEADER "io.bd")


add_library(os SHARED os.cpp)
target_link_libraries(os bondlib)
set_target_properties(os PROPERTIES PUBLIC_HEADER "os.bd")


add_library(gc SHARED gc.cpp)
target_link_libraries(gc bondlib)
set_target_properties(gc PROPERTIES PUBLIC_HEADER "gc.bd")


add_library(isolate SHARED isolate.cpp)
target_link_libraries(isolate bondlib)
set_target_properties(isolate PROPERTIES PUBLIC_HEADER "isolate.bd")

INSTALL(TARGETS io os gc isolate
        LIBRARY DESTINATION libraries
        RUNTIME DESTINATION libraries
        PUBLIC_HEADER DESTINATION libraries)