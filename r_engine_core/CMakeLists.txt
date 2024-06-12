cmake_minimum_required(VERSION 3.12)

set(CORE_PROJECT_NAME r_engine_core)
project(${CORE_PROJECT_NAME})

# PUBLIC INCLUDES
set(CORE_PUBLIC_INCLUDES
    includes/r_engine_core/Application.hpp
    includes/r_engine_core/Log.hpp
    includes/r_engine_core/Event.hpp
    includes/r_engine_core/Camera3D.hpp
    includes/r_engine_core/Keys.hpp
    includes/r_engine_core/Input.hpp
    includes/r_engine_core/NodeApplication.hpp
)

# PRIVATE INCLUDES
set(CORE_PRIVATE_INCLUDES
    src/r_engine_core/Window.hpp
    src/r_engine_core/Modules/UIModule.hpp

    # OpenGL Rendering
    src/r_engine_core/Rendering/OpenGL/ShaderProgram3D.hpp
    src/r_engine_core/Rendering/OpenGL/VertexBuffer.hpp
    src/r_engine_core/Rendering/OpenGL/VertexArray.hpp
    src/r_engine_core/Rendering/OpenGL/IndexBuffer.hpp
    src/r_engine_core/Rendering/OpenGL/Texture2D.hpp
    src/r_engine_core/Rendering/OpenGL/Renderer_OpenGL.hpp

    # Node OpenGL Rendering
    src/r_engine_core/Rendering/NodeOpenGL/Buffers.hpp
    src/r_engine_core/Rendering/NodeOpenGL/Camera.hpp
    src/r_engine_core/Rendering/NodeOpenGL/Lights.hpp
    src/r_engine_core/Rendering/NodeOpenGL/Model.hpp
    src/r_engine_core/Rendering/NodeOpenGL/Scene.hpp
    src/r_engine_core/Rendering/NodeOpenGL/Shader.hpp
    src/r_engine_core/Rendering/NodeOpenGL/Texture.hpp
)

# PRIVATE SOURCES
set(CORE_PRIVATE_SOURCES
    src/r_engine_core/Application.cpp
    src/r_engine_core/Window.cpp
    src/r_engine_core/Camera3D.cpp
    src/r_engine_core/Modules/UIModule.cpp
    src/r_engine_core/Input.cpp
    src/r_engine_core/NodeApplication.cpp

    # OpenGL Rendering
    src/r_engine_core/Rendering/OpenGL/ShaderProgram3D.cpp
    src/r_engine_core/Rendering/OpenGL/VertexBuffer.cpp
    src/r_engine_core/Rendering/OpenGL/VertexArray.cpp
    src/r_engine_core/Rendering/OpenGL/IndexBuffer.cpp
    src/r_engine_core/Rendering/OpenGL/Texture2D.cpp
    src/r_engine_core/Rendering/OpenGL/Renderer_OpenGL.cpp

    # Node OpenGL Rendering
    src/r_engine_core/Rendering/NodeOpenGL/Camera.cpp
    src/r_engine_core/Rendering/NodeOpenGL/Lights.cpp
    src/r_engine_core/Rendering/NodeOpenGL/Model.cpp
    src/r_engine_core/Rendering/NodeOpenGL/Scene.cpp
    src/r_engine_core/Rendering/NodeOpenGL/Shader.cpp
    src/r_engine_core/Rendering/NodeOpenGL/Texture.cpp
)

set(CORE_ALL_SOURCES
    ${CORE_PUBLIC_INCLUDES}
    ${CORE_PRIVATE_INCLUDES}
    ${CORE_PRIVATE_SOURCES}
)

add_library( ${CORE_PROJECT_NAME} STATIC ${CORE_ALL_SOURCES})

target_include_directories(${CORE_PROJECT_NAME} PUBLIC includes)
target_include_directories(${CORE_PROJECT_NAME} PRIVATE src)

target_compile_features(${CORE_PROJECT_NAME} PUBLIC cxx_std_17)

# OGL, GLFW LINK
#find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
target_link_libraries(${CORE_PROJECT_NAME} PRIVATE glfw)

# GLAD LINK
add_subdirectory(../external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)
target_link_libraries(${CORE_PROJECT_NAME} PRIVATE glad)

# SPDLOG LINK
add_subdirectory(../external/spdlog ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
target_link_libraries(${CORE_PROJECT_NAME} PRIVATE spdlog)

# IMGUI LINK
set(IMGUI_INCLUDES
    ../external/imgui/imgui.h
    ../external/imgui/backends/imgui_impl_glfw.h
    ../external/imgui/backends/imgui_impl_opengl3.h
)

set(IMGUI_SOURCES
    ../external/imgui/imgui.cpp
    ../external/imgui/backends/imgui_impl_glfw.cpp
    ../external/imgui/backends/imgui_impl_opengl3.cpp
    ../external/imgui/imgui_demo.cpp
    ../external/imgui/imgui_draw.cpp
    ../external/imgui/imgui_tables.cpp
    ../external/imgui/imgui_widgets.cpp
)

add_library( ImGUI STATIC
    ${IMGUI_INCLUDES}
    ${IMGUI_SOURCES}
)

target_include_directories(ImGUI PUBLIC ../external)
target_include_directories(ImGUI PUBLIC ../external/imgui)

target_link_libraries(ImGUI PRIVATE glfw glad)
target_link_libraries(${CORE_PROJECT_NAME} PRIVATE ImGUI)

# GLM LINK
add_subdirectory(../external/glm ${CMAKE_CURRENT_BINARY_DIR}/glm)
target_link_libraries(${CORE_PROJECT_NAME} PRIVATE glm)

