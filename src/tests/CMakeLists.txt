project(FCTests)

add_custom_target(
        test.nes
        COMMAND ca65 ${CMAKE_CURRENT_SOURCE_DIR}/test.asm -o test.o
        COMMAND ld65 -t nes -o test.nes test.o
)
configure_file(nestest.nes nestest.nes COPYONLY)
configure_file(nestest.log nestest.log COPYONLY)

configure_file(Super\ Mario\ Bros\ \(E\).nes Super\ Mario\ Bros\ \(E\).nes COPYONLY)

add_executable(FCtests
        FCTests.cpp)
add_dependencies(FCtests test.nes)
target_link_libraries(FCtests gtest gtest_main FC SDL2)

add_executable(sdltest
        sdltest.cpp)
target_link_libraries(sdltest SDL2)
