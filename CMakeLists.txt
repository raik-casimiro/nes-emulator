cmake_minimum_required(VERSION 3.10)
project(NesEmulator VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/emulator.cpp
)

add_executable(emulator ${SOURCES})

option(ENABLE_TESTS "Enable unit tests" ON)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)

endif()
