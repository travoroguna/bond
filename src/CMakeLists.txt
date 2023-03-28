cmake_minimum_required(VERSION 3.22)
project(bondlib)

set(CMAKE_CXX_STANDARD 23)
add_subdirectory(external/fmt)


add_library(bondlib lexer.cpp lexer.h context.cpp context.h span.h ast.cpp ast.h parser.cpp parser.h code.cpp code.h vm.cpp vm.h gc.cpp gc.h nodevisitor.cpp nodevisitor.h object.h object.cpp)
target_link_libraries(bondlib fmt::fmt)