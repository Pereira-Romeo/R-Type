cmake_minimum_required(VERSION 3.20)
project(r-type_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED CONFIG)
find_package(Threads REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

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
${CMAKE_SOURCE_DIR}/common/src/Protocol.cpp
)
target_include_directories(r-type_client PRIVATE ${CMAKE_SOURCE_DIR}/common/include ${LUA_INCLUDE_DIR})
target_include_directories(r-type_client PRIVATE
    include
${CMAKE_SOURCE_DIR}/frameBufferHandler/include
${CMAKE_SOURCE_DIR}/jsonHandler
${CMAKE_SOURCE_DIR}
)

target_compile_definitions(r-type_client PRIVATE ASIO_STANDALONE)
target_compile_definitions(r-type_client PRIVATE
    $<$<CONFIG:Debug>:ASSETS_ROOT="${CMAKE_SOURCE_DIR}">)

target_link_libraries(r-type_client PRIVATE
        project_options
        game_engine
        SDL3::SDL3
        SDL3_image::SDL3_image
        Threads::Threads
        imgui::imgui
        nlohmann_json::nlohmann_json
        sol2::sol2
        ${LUA_LIBRARIES}
)

add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:r-type_client>/assets)