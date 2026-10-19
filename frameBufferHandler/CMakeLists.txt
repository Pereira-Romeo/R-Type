cmake_minimum_required(VERSION 3.16)
project(BufferHandler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(buffer_test
    src/main.cpp
    src/FrameBufferHandler.cpp
)

target_include_directories(buffer_test PRIVATE include)
target_compile_options(buffer_test PRIVATE -Wall -Wextra)
