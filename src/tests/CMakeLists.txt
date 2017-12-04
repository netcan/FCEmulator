project(FCTests)

add_executable(FCtests
        FCTests.cpp)

target_link_libraries(FCtests gtest gtest_main)
target_link_libraries(FCtests FC)
