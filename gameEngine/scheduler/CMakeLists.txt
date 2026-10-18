add_library(scheduler INTERFACE)
add_library(engine::scheduler ALIAS scheduler)

target_include_directories(scheduler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(scheduler INTERFACE engine::ecs)