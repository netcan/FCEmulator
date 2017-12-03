cmake_minimum_required(VERSION 3.8)
project(fcemu)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp cpu.cpp cpu.h)
add_executable(fcemu ${SOURCE_FILES})