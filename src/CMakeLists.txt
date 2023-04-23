cmake_minimum_required(VERSION 3.22)
project(bondlib)

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
)

add_subdirectory(libs)


add_library(bondlib lexer.cpp lexer.h context.cpp context.h span.h ast.cpp ast.h
        parser.cpp parser.h code.cpp code.h vm.cpp vm.h gc.cpp gc.h nodevisitor.cpp
        nodevisitor.h object.h object.cpp api.h api.cpp builtins.cpp bond.h lsp/lsp.cpp
        lsp/lsp.h lsp/lsp.h lsp/rpc.cpp lsp/rpc.h lsp/rpc.h result.h
        objects/bool.cpp objects/float.cpp objects/integer.cpp objects/string.cpp objects/nil.cpp
        objects/struct.cpp objects/instance.cpp objects/list.cpp objects/map.cpp object_helpers.h)

target_link_libraries(bondlib fmt::fmt-header-only nlohmann_json plibsys)

set_target_properties(bondlib PROPERTIES PUBLIC_HEADER "lexer.h;context.h;span.h;ast.h;parser.h;code.h;vm.h;gc.h;nodevisitor.h;object.h;api.h;bond.h;lsp/lsp.h")

INSTALL(TARGETS bondlib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/bondlib
        )
