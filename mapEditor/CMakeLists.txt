cmake_minimum_required(VERSION 3.14)
project(level_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.0
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(json SDL3)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.9
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/../client/src/Core/SceneManager.cpp"
    "${CMAKE_SOURCE_DIR}/../client/src/UI/Text.cpp"
    "${CMAKE_SOURCE_DIR}/../client/src/UI/ButtonList.cpp"
)
add_executable(editor_test main.cpp ${SOURCES})

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${imgui_SOURCE_DIR}/misc/cpp
)
target_link_libraries(imgui PUBLIC SDL3::SDL3)


target_link_libraries(editor_test PRIVATE
    imgui
    nlohmann_json::nlohmann_json
    SDL3::SDL3
)

target_include_directories(editor_test PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/../client/include
)

