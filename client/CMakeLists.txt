add_executable(r-type_client)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
target_sources(r-type_client PRIVATE ${CLIENT_SOURCES})

target_include_directories(r-type_client PRIVATE include)
target_link_libraries(r-type_client PRIVATE project_options game_engine)
