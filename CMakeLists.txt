cmake_minimum_required(VERSION 3.10)

project(chip8)


set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH  "C:\\msys64\\mingw64\\lib\\cmake\\SDL2")

find_package(SDL2 REQUIRED)

add_executable(chip8_emulator src/main.cpp)

target_include_directories(chip8_emulator PRIVATE "C:\\msys64\\mingw64\\include")

add_library(chip8 ${CMAKE_SOURCE_DIR}//src//chip8.cpp)

target_link_libraries(chip8_emulator PRIVATE chip8 )
# if(WIN32)

# endif(WIN32)

