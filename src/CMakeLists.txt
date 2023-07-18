cmake_minimum_required(VERSION 3.22)
project(bond-lib)

set(CMAKE_CXX_STANDARD 23)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(FMT_INSTALL OFF CACHE INTERNAL "")

CPMAddPackage(
        NAME nlohmann_json
        VERSION 3.11.2
        # the git repo is incredibly large, so we download the archived include directory
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip
        URL_HASH SHA256=e5c7a9f49a16814be27e4ed0ee900ecd0092bfb7dbfca65b5a421b774dccaaed
)

if (nlohmann_json_ADDED)
    add_library(nlohmann_json INTERFACE IMPORTED)
    target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR}/include)
endif ()

CPMAddPackage(
        NAME fmtlib
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 9.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL YES
)

if (MSVC)
    CPMAddPackage(
            NAME libatomic_ops
            GITHUB_REPOSITORY ivmai/libatomic_ops
            GIT_TAG v7.8.0
            OPTIONS "enable_gpl ON"
            EXCLUDE_FROM_ALL YES
    )
endif ()

CPMAddPackage(
        NAME bdwgc
        GITHUB_REPOSITORY ivmai/bdwgc
        GIT_TAG v8.2.4
        OPTIONS "enable_docs OFF" "enable_cplusplus ON" "BUILD_SHARED_LIBS OFF" "enable_gc_debug OFF" "enable_large_config ON"
        EXCLUDE_FROM_ALL YES
)


add_subdirectory(libs)

add_library(bond-lib STATIC compiler/lexer.cpp compiler/lexer.h compiler/context.cpp compiler/context.h compiler/span.h compiler/ast.cpp compiler/ast.h
        compiler/parser.cpp compiler/parser.h compiler/codegen.cpp compiler/codegen.h vm.cpp vm.h gc.cpp gc.h compiler/nodevisitor.cpp
        compiler/nodevisitor.h object.h object.cpp api.h api.cpp builtins.cpp bond.h
        objects/bool.cpp objects/float.cpp objects/integer.cpp objects/string.cpp objects/nil.cpp
        objects/struct.cpp objects/instance.cpp objects/list.cpp objects/map.cpp object_helpers.h objects/future.cpp md5.cpp md5.h objects/nativestruct.cpp objects/code.cpp objects/function.cpp objects/module.cpp objects/result.cpp objects/closure.cpp debug.cpp debug.h conv.hpp conv.hpp import.cpp import.h core/conversions.cpp core/conversions.h core/core.cpp core/core.h engine.cpp engine.h engine.h)

target_link_libraries(bond-lib gc gccpp fmt::fmt-header-only nlohmann_json)

if (MSVC)
    target_link_libraries(gc atomic_ops)
endif ()

#set_target_properties(bond-lib PROPERTIES PUBLIC_HEADER "lexer.h;context.h;span.h;ast.h;parser.h;code.h;vm.h;gc.h;nodevisitor.h;object.h;api.h;bond.h;object_helpers.h;builtins.h;result.h;globs.h;")

INSTALL(TARGETS bond-lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/bondlib
)

#INSTALL(TARGETS fmt)
#INSTALL(FILES bondlibConfig.cmake DESTINATION lib/cmake/bondlib)
