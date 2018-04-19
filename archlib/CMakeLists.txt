cmake_minimum_required(VERSION 3.4)
project(arch2-5a)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")


AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} source)
add_library(arch2-5a ${source})
