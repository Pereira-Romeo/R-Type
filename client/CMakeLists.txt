cmake_minimum_required(VERSION 3.20)
project(r-type_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED CONFIG)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(r-type_client ${SOURCES}
        include/MainMenu.hpp)
target_include_directories(r-type_client PRIVATE src)
target_link_libraries(r-type_client PRIVATE SDL3::SDL3)
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

target_sources(r-type_client PRIVATE
    ${CLIENT_SOURCES}
    ${CMAKE_SOURCE_DIR}/frameBufferHandler/src/FrameBufferHandler.cpp
)

target_include_directories(r-type_client PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/frameBufferHandler/include
)

target_link_libraries(r-type_client PRIVATE project_options game_engine SDL3::SDL3)
