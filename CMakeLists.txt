cmake_minimum_required(VERSION 3.22)
project(bond VERSION 0.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


OPTION(BOND_DEBUG "Debug mode" OFF)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME CPMLicenses.cmake
        GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
        VERSION 0.0.7
        EXCLUDE_FROM_ALL YES
)

CPMAddPackage(
        NAME argumentum
        GITHUB_REPOSITORY mmahnic/argumentum
        GIT_TAG v0.3.2
        EXCLUDE_FROM_ALL YES
)


add_subdirectory(src)
add_subdirectory(src/lsp)
add_subdirectory(src/docserver)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set(WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/bond.rc)

    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/bin")
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    if (MSVC)
        set(CMAKE_CXX_FLAGS "/W4 /cgthreads8 /EHsc /analyze")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wconversion  -Wpedantic -fexceptions -pthread -dl")
    endif ()

    add_executable(bond src/main.cpp ${WIN32_RESOURCES})
    message("Windows")
else ()
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wconversion -Wpedantic -fexceptions -pthread -Wno-changes-meaning")
    add_executable(bond src/main.cpp)
    message("Linux")
endif ()



if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -pthread")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    TARGET_COMPILE_OPTIONS(bond PRIVATE -s --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/shell_minimal.html)
endif()


target_link_libraries(bond PRIVATE bond-lib Argumentum::headers)

add_executable(bootstrap src/bootstrap.cpp)
target_link_libraries(bootstrap PRIVATE bond-lib)

install(TARGETS bootstrap DESTINATION bin)


cpm_licenses_create_disclaimer_target(
        write-licenses "${CMAKE_CURRENT_BINARY_DIR}/third_party.txt" "${CPM_PACKAGES}"
)

install(TARGETS bond)

if (BOND_DEBUG)
    message("Debug mode")
    target_compile_definitions(bond-lib PRIVATE DEBUG)
endif ()


set(CPACK_PACKAGE_NAME "bond")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/third_party.txt")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
include(CPack)


include(CTest)
enable_testing()
add_subdirectory(tests)
