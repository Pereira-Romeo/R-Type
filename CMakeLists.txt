cmake_minimum_required(VERSION 3.28)
project(r-type LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Dependencies (installed by vcpkg from vcpkg.json)
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Options shared by every target: warnings + asio setup
add_library(project_options INTERFACE)
target_link_libraries(project_options INTERFACE asio::asio Threads::Threads)
target_compile_definitions(
    project_options INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED
)

if(MSVC)
    target_compile_options(
        project_options INTERFACE /W4 /permissive- /utf-8 /Zc:__cplusplus
    )
else()
    target_compile_options(project_options INTERFACE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
    # asio wants to know the minimum Windows version (0x0A00 = Windows 10)
    target_compile_definitions(project_options INTERFACE _WIN32_WINNT=0x0A00)
    target_link_libraries(project_options INTERFACE ws2_32 mswsock)
endif()

add_subdirectory(gameEngine)
add_subdirectory(server)
add_subdirectory(client)
