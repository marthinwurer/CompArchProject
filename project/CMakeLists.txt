cmake_minimum_required(VERSION 3.5)
project(p1)

set(CMAKE_CXX_STANDARD 98)

include_directories(${CMAKE_SOURCE_DIR}/archlib)
link_directories(${CMAKE_SOURCE_DIR}/archlib)

add_executable(z88 z88.cpp globals.cpp globals.h z88.cpp)
target_link_libraries(z88 arch2-5a)