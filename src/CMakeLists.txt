cmake_minimum_required(VERSION 3.22)
project(bondlib)

set(CMAKE_CXX_STANDARD 23)

add_library(bondlib lexer.cpp lexer.h context.cpp context.h span.h ast.cpp ast.h parser.cpp parser.h code.cpp code.h vm.cpp vm.h )