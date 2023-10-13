cmake_minimum_required(VERSION 3.26)
project(namumark C)

add_library(namumark SHARED
        src/ast.c
        src/ast.h
        src/stack.c
        src/stack.h
        src/parser.c
        src/parser.h
        src/parser_utils.c)