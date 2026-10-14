add_library(game_engine STATIC) #STATIC to make it a library

file(GLOB_RECURSE GAME_ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
target_sources(game_engine PRIVATE ${GAME_ENGINE_SOURCES})

#set to public for server and client to link to it
target_include_directories(game_engine PUBLIC include)
target_link_libraries(game_engine PUBLIC project_options)
