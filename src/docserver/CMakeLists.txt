cmake_minimum_required(VERSION 3.22)
project(docserver)


set(CMAKE_CXX_STANDARD 23)
set(SOURCES doc.cpp doc.h main.cpp
        htmx.h)

add_executable(docserver ${SOURCES})
target_link_libraries(docserver PRIVATE bond-lib httplib::httplib)
install(TARGETS docserver DESTINATION bin)