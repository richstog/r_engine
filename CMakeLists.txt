cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME r_engine)

add_subdirectory(r_engine_core)
add_subdirectory(r_engine_editor)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT r_engine_editor)
