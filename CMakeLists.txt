cmake_minimum_required(VERSION 3.22)
project(bond)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(PLIBSYS_TESTS OFF CACHE BOOL "" FORCE)
set(PLIBSYS_BUILD_DOC OFF CACHE BOOL "" FORCE)


include(cmake/CPM.cmake)

CPMAddPackage(
        NAME plibsys
        GITHUB_REPOSITORY saprykin/plibsys
        GIT_TAG 0.0.4
        EXCLUDE_FROM_ALL YES
        GIT_SHALLOW ON

)

CPMAddPackage(
        NAME CPMLicenses.cmake
        GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
        VERSION 0.0.7
        EXCLUDE_FROM_ALL YES
)
add_subdirectory(src)
add_subdirectory(src/lsp)


if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set(WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/bond.rc)
    add_executable(bond src/main.cpp ${WIN32_RESOURCES})
    set(CMAKE_CXX_FLAGS "/Wall /W4 /cgthreads8  /EHsc")
    message("Windows")
else ()
    add_executable(bond src/main.cpp)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -fexceptions -pthread -dl")
    message("Linux")
endif ()

target_link_libraries(bond bondlib plibsysstatic)


add_executable(libtool src/libtool/libtool.cpp)
target_link_libraries(libtool bondlib plibsysstatic)

cpm_licenses_create_disclaimer_target(
        write-licenses "${CMAKE_CURRENT_BINARY_DIR}/third_party.txt" "${CPM_PACKAGES}"
)

install(TARGETS libtool)
install(TARGETS bond)
#INSTALL(TARGETS plibsys RUNTIME DESTINATION .)


set(CPACK_PACKAGE_NAME "bond")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)

enable_testing()
add_subdirectory(tests)
