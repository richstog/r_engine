// #pragma once

// #include <r_engine_core/Rendering/OpenGL/VertexBuffer.hpp>
// #include <r_engine_core/Rendering/OpenGL/IndexBuffer.hpp>
// #include <r_engine_core/Rendering/OpenGL/VertexArray.hpp>
// #include <r_engine_core/Rendering/OpenGL/ShaderProgram.hpp>
// #include <r_engine_core/Camera3D.hpp>

// namespace r_engine
// {

//     class Object3D
//     {
//         private:

//         glm::vec3 m_position = glm::vec3(0.f, 0.f, 0.f);
//         glm::vec3 m_scale = glm::vec3(1.f, 1.f, 1.f);
//         float m_rotate_x = 0;
//         float m_rotate_y = 0;
//         float m_rotate_z = 0;
//         glm::mat4 m_model_matrix = glm::mat4(1.f);
//         glm::vec3 m_color_vector = glm::vec3(1.f, 0.f, 0.f);

//         GLfloat* m_position_norm_uv;
//         GLuint* m_indexes;

//         const char* m_vertex_shader;
//         const char* m_fragment_shader;

//         ShaderProgram* m_shader_program;
//         VertexBuffer* m_vertex_buffer;
//         IndexBuffer* m_index_buffer;
//         VertexArray* m_vertex_array;

//         public:
//         Object3D(
//             GLfloat* position_norm_uv,
//             GLuint* indexes,
//             const char* vertex_shader,
//             const char* fragment_shader,
//             glm::vec3 position,
//             glm::vec3 color_vector
//         );

//         void updateModelMatrix();
//         void draw(Camera3D Camera3D);
//         ~Object3D();
//     };
// }