cmake_minimum_required(VERSION 3.22)
project(bond)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CPack)
add_subdirectory(src)


if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set(WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/bond.rc)
    add_executable(bond src/main.cpp ${WIN32_RESOURCES})
    message("Windows")
else ()
    add_executable(bond src/main.cpp)
    message("Linux")
endif ()

target_link_libraries(bond bondlib)

install(TARGETS bond DESTINATION bin)

set(CPACK_PACKAGE_NAME "bond")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)

enable_testing()
add_subdirectory(tests)
