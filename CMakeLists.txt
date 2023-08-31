cmake_minimum_required(VERSION 3.15)
# library name
project(libnamumark)
# build library(include library/src)
add_subdirectory(library)
# build test
add_subdirectory(test)