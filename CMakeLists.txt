cmake_minimum_required(VERSION 3.26)
project(libnamumark C)

set(CMAKE_C_STANDARD 23)

add_library(libnamumark SHARED src/library.c
        src/ast.c
        src/ast.h
        src/stack.c
        src/stack.h
        src/parser.c
        src/parser.h)
