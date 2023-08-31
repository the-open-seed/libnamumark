cmake_minimum_required(VERSION 3.26)
project(googletest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(tests src/parser_test.cpp)
target_link_libraries(tests namumark gtest_main)
# add include directories

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/library/src)
include(GoogleTest)
gtest_discover_tests(tests)

# run tests
add_custom_target(run_tests ALL COMMAND tests)