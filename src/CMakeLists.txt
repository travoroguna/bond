cmake_minimum_required(VERSION 3.22)
project(bondlib)

set(CMAKE_CXX_STANDARD 23)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(FMT_INSTALL OFF CACHE INTERNAL "")

add_subdirectory(external/fmt)
add_subdirectory(external/json)


add_library(bondlib lexer.cpp lexer.h context.cpp context.h span.h ast.cpp ast.h parser.cpp parser.h code.cpp code.h vm.cpp vm.h gc.cpp gc.h nodevisitor.cpp nodevisitor.h object.h object.cpp api.h api.cpp builtins.cpp bond.h lsp/lsp.cpp lsp/lsp.h lsp/lsp.h)
target_link_libraries(bondlib fmt::fmt-header-only nlohmann_json::nlohmann_json)

set_target_properties(bondlib PROPERTIES PUBLIC_HEADER "lexer.h;context.h;span.h;ast.h;parser.h;code.h;vm.h;gc.h;nodevisitor.h;object.h;api.h;bond.h;lsp/lsp.h")

INSTALL(TARGETS bondlib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)