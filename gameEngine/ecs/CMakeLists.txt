add_library(ecs INTERFACE)
add_library(engine::ecs ALIAS ecs)
target_include_directories(ecs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)