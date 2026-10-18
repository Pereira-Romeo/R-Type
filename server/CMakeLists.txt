add_executable(r-type_server)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
target_sources(r-type_server PRIVATE
    ${SERVER_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/Protocol.cpp
)

target_include_directories(r-type_server PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)
target_link_libraries(r-type_server PRIVATE project_options game_engine)
