cmake_minimum_required(VERSION 3.12)

set(EDITOR_PROJECT_NAME r_engine_editor)

add_executable(${EDITOR_PROJECT_NAME}
    src/main.cpp
)

# Возможные библиотеки: glfw, glad, spdlog, ImGUI, glm
target_link_libraries(${EDITOR_PROJECT_NAME} r_engine_core ImGUI glm)
target_compile_features(${EDITOR_PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)