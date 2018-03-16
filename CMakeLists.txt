cmake_minimum_required(VERSION 3.7)

project(object_pool_test VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. SRC)
file(GLOB INC *.h)

add_executable(object_pool_test ${SRC} ${INC})