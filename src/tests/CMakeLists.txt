project(FCTests)

add_custom_target(
        test.nes
        COMMAND ca65 ${CMAKE_CURRENT_SOURCE_DIR}/test.asm -o test.o
        COMMAND ld65 -t nes -o test.nes test.o
)
configure_file(nestest.nes nestest.nes COPYONLY)
configure_file(nestest.log nestest.log COPYONLY)

add_executable(FCtests
        FCTests.cpp)
add_dependencies(FCtests test.nes)
target_link_libraries(FCtests gtest gtest_main)
target_link_libraries(FCtests FC)
