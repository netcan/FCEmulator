project(FCTests)

add_executable(FCtests
        FCTests.cpp)

configure_file(nestest.nes nestest.nes COPYONLY)
target_link_libraries(FCtests gtest gtest_main)
target_link_libraries(FCtests FC)
