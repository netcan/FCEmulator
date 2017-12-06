cmake_minimum_required(VERSION 3.8)
project(FCEmu)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)

ExternalProject_Add(
        googletest
        PREFIX lib
        GIT_REPOSITORY https://github.com/google/googletest.git
)

set(SOURCE_FILES main.cpp Cpu.cpp Cpu.h Cartridge.cpp Cartridge.h Base.h Famicom.cpp Famicom.h)
add_library(FC ${SOURCE_FILES})
add_executable(FCemu ${SOURCE_FILES})
target_link_libraries(FCemu FC)

add_subdirectory(tests)
