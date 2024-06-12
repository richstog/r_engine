#define GLFW_INCLUDE_NONE

#include <r_engine_core/Application.hpp>
#include <r_engine_core/Window.hpp>
#include <r_engine_core/Event.hpp>

#include <r_engine_core/Rendering/OpenGL/ShaderProgram3D.hpp>
#include <r_engine_core/Rendering/OpenGL/VertexBuffer.hpp>
#include <r_engine_core/Rendering/OpenGL/IndexBuffer.hpp>
#include <r_engine_core/Rendering/OpenGL/VertexArray.hpp>
#include <r_engine_core/Camera3D.hpp>
#include <r_engine_core/Input.hpp>
#include <r_engine_core/Rendering/OpenGL/Texture2D.hpp>

#include <r_engine_core/Rendering/OpenGL/Renderer_OpenGL.hpp>
#include <r_engine_core/Modules/UIModule.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <glm/mat3x3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include "Rendering/OpenGL/Renderer_OpenGL.hpp"

namespace r_engine
{
    // Массив данных вершин
    GLfloat cube_pos_norm_uv[] = {
        // normal - вектор перпендикулярный поверхности
        // position---------normal--------------UV----------index
        // front
        -1.f, -1.f, -1.f,   -1.f, 0.f, 0.f,     0.f, 0.f,   // 0
        -1.f, 1.f, -1.f,    -1.f, 0.f, 0.f,     10.f, 0.f,   // 1
        -1.f, 1.f, 1.f,     -1.f, 0.f, 0.f,     10.f, 10.f,   // 2
        -1.f, -1.f, 1.f,    -1.f, 0.f, 0.f,     0.f, 10.f,   // 3

        // back
        1.f, -1.f, -1.f,    1.f, 0.f, 0.f,      10.f, 0.f,   // 4
        1.f, 1.f, -1.f,     1.f, 0.f, 0.f,      0.f, 0.f,   // 5
        1.f, 1.f, 1.f,      1.f, 0.f, 0.f,      0.f, 10.f,   // 6
        1.f, -1.f, 1.f,     1.f, 0.f, 0.f,      10.f, 10.f,   // 7

        // right
        -1.f, 1.f, -1.f,    0.f, 1.f, 0.f,      0.f, 0.f,   // 8
        1.f, 1.f, -1.f,     0.f, 1.f, 0.f,      10.f, 0.f,   // 9
        1.f, 1.f, 1.f,      0.f, 1.f, 0.f,      10.f, 10.f,   // 10
        -1.f, 1.f, 1.f,     0.f, 1.f, 0.f,      0.f, 10.f,   // 11

        // left
        -1.f, -1.f, -1.f,   0.f, -1.f, 0.f,     10.f, 0.f,   // 12
        1.f, -1.f, -1.f,    0.f, -1.f, 0.f,     0.f, 0.f,   // 13
        1.f, -1.f, 1.f,     0.f, -1.f, 0.f,     0.f, 10.f,   // 14
        -1.f, -1.f, 1.f,    0.f, -1.f, 0.f,     10.f, 10.f,   // 15

        // top
        -1.f, -1.f, 1.f,    0.f, 0.f, 1.f,      0.f, 0.f,   // 16
        -1.f, 1.f, 1.f,     0.f, 0.f, 1.f,      10.f, 0.f,   // 17
        1.f, 1.f, 1.f,      0.f, 0.f, 1.f,      10.f, 10.f,   // 18
        1.f, -1.f, 1.f,     0.f, 0.f, 1.f,      0.f, 10.f,   // 19

        // bottom
        -1.f, -1.f, -1.f,   0.f, 0.f, -1.f,     0.f, 10.f,   // 20
        -1.f, 1.f, -1.f,    0.f, 0.f, -1.f,     10.f, 10.f,   // 21
        1.f, 1.f, -1.f,     0.f, 0.f, -1.f,     10.f, 0.f,   // 22
        1.f, -1.f, -1.f,    0.f, 0.f, -1.f,     0.f, 0.f,   // 23
    };

    // Массив индексов вершин
    GLuint indexes[] = {
        0, 1, 2, 2, 3, 0,       // front
        4, 5, 6, 6, 7, 4,       // back
        8, 9, 10, 10, 11, 8,    // right
        12, 13, 14, 14, 15, 12, // left
        16, 17, 18, 18, 19, 16, // top
        20, 21, 22, 22, 23, 20  // bottom
    };

    // Создание текстуры с квадратами
    void generate_quads_texture (
        unsigned char* data,
        const unsigned int width,
        const unsigned int height
    )
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            for (unsigned int y = 0; y < height; ++y)
            {
                if ((x < width / 2 && y < height / 2) || x >= width / 2 && y >= height / 2)
                {
                    data[3 * (x + width * y) + 0] = 0;
                    data[3 * (x + width * y) + 1] = 0;
                    data[3 * (x + width * y) + 2] = 0;
                } else
                {
                    data[3 * (x + width * y) + 0] = 255;
                    data[3 * (x + width * y) + 1] = 255;
                    data[3 * (x + width * y) + 2] = 255;
                }
            }
        }
    }

    /* Шейдеры 3D объектов */

    // Вершинный шейдер
    const char* vertex_shader =
        R"(
            #version 460
            layout(location = 0) in vec3 vertex_position;
            layout(location = 1) in vec3 vertex_normal;
            layout(location = 2) in vec2 texture_coord;

            uniform mat4 model_view_matrix;
            uniform mat4 mvp_matrix;
            uniform mat3 normal_matrix;

            out vec2 tex_coord_smile;
            out vec2 tex_coord_none;
            out vec3 frag_normal_eye;
            out vec3 frag_position_eye;

            void main() {

                /* OUT INIT */
                frag_normal_eye = normal_matrix * vertex_normal;
                frag_position_eye = vec3(model_view_matrix * vec4(vertex_position, 1.0));
                tex_coord_smile = texture_coord;

                /* FUNCTIONAL */
                gl_Position = mvp_matrix * vec4(vertex_position, 1.0);
            }
        )";

    // Фрагментный шейдер
    const char* fragment_shader =
        R"(
            #version 460

            in vec2 tex_coord_smile;
            in vec2 tex_coord_none;
            in vec3 frag_position_eye;
            in vec3 frag_normal_eye;

            layout (binding = 0) uniform sampler2D InTexture_Smile;
            layout (binding = 1) uniform sampler2D InTexture_none;

            uniform vec3 light_position_eye;
            uniform vec3 light_color;
            uniform float ambient_factor;
            uniform float diffuse_factor;
            uniform float specular_factor;
            uniform float shininess;
            
            out vec4 frag_color;

            void main() {
                // ambient
                vec3 ambient = ambient_factor * light_color;

                // diffuse
                vec3 normal = normalize(frag_normal_eye);
                vec3 light_dir = normalize(light_position_eye - frag_position_eye);
                vec3 diffuse = diffuse_factor * light_color * max(dot(normal, light_dir), 0.f);

                // specular
                vec3 view_dir = normalize(-frag_position_eye);
                vec3 reflect_dir = reflect(-light_dir, normal);
                float specular_value = pow(max(dot(view_dir, reflect_dir), 0.f), shininess);
                vec3 specular = specular_factor * specular_value * light_color;

                frag_color = texture(InTexture_Smile, tex_coord_smile) * vec4(ambient + diffuse + specular, 1.f);
            }
        )";

    /* Шейдеры светого объекта */

    // Вершинный шейдер
    const char* light_vertex_shader =
        R"(
            #version 460
            layout(location = 0) in vec3 vertex_position;
            layout(location = 1) in vec3 vertex_normal;
            layout(location = 2) in vec2 texture_coord;

            uniform mat4 mvp_matrix; // model view projection

            out vec3 color;

            void main() {
               //color = vertex_color;
               gl_Position = mvp_matrix * vec4(vertex_position * 0.1f, 1.0);
            }
        )";

    // Фрагментный шейдер
    const char* light_fragment_shader =
        R"(
            #version 460

            out vec4 frag_color;

            uniform vec3 light_color;

            void main() {
                frag_color = vec4(light_color, 1.f);
            }
        )";
    
    // Инициализация освещения
    std::unique_ptr<ShaderProgram3D> p_light_shader_program;

    // Инициализация 3D объектов
    std::unique_ptr<ShaderProgram3D> p_shader_program;
    std::unique_ptr<VertexBuffer> p_cube_positions_vbo;
    std::unique_ptr<IndexBuffer> p_cube_index_buffer;
    std::unique_ptr<Texture2D> p_texture_smile;
    std::unique_ptr<Texture2D> p_texture_none;
    std::unique_ptr<VertexArray> p_cube_vao;
    
    float scale[3] = {1.f, 1.f, 1.f};
    float rotate = 0.f;
    float translate[3] = {0.f, 0.f, 0.f};

    float m_background_color[4] = {0.1f, 0.33f, 0.33f, 0.f};

    Application::Application()
    {
        LOG_INFO("Starting Application");
    }

    Application::~Application()
    {
        LOG_INFO("Closing Application");
    }

    void Application::draw()
    {
        //----------------------------------//
        // Рендер OpenGL

        Renderer_OpenGL::set_clear_color(
            m_background_color[0],
            m_background_color[1],
            m_background_color[2],
            m_background_color[3]
        );
        Renderer_OpenGL::clear();

        p_shader_program->bind();

        p_shader_program->setVec3("light_position_eye",
            glm::vec3(camera.get_view_matrix() * glm::vec4(light_position[0],
            light_position[1],
            light_position[2],
            1.f))
        );
        p_shader_program->setVec3("light_color", glm::vec3(light_color[0], light_color[1], light_color[2]));
        p_shader_program->setFloat("ambient_factor", light_ambient_factor);
        p_shader_program->setFloat("diffuse_factor", light_diffuse_factor);
        p_shader_program->setFloat("specular_factor", light_specular_factor);
        p_shader_program->setFloat("shininess", light_shininess);

        Renderer_OpenGL::draw(*p_cube_vao);


        // 3D объекты
        for (const glm::vec3 current_position : positions)
        {
            // Изменение матрицы перемещения объектов
            glm::mat4 translate_m = glm::translate(glm::mat4(1.f), current_position);
            const glm::mat4 model_view_matrix = camera.get_view_matrix() * translate_m;

            // Настройка шейдера объектов
            p_shader_program->setMatrix4("model_view_matrix", model_view_matrix);
            p_shader_program->setMatrix4("mvp_matrix", camera.get_projection_matrix() * model_view_matrix);
            p_shader_program->setMatrix3("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model_view_matrix))));

            // Отрисовка объекта
            Renderer_OpenGL::draw(*p_cube_vao);
        }

        // Освещение
        {
            p_light_shader_program->bind();
            glm::mat4 translate_m(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                light_position[0], light_position[1], light_position[2], 1
            );
            p_light_shader_program->setMatrix4("mvp_matrix", camera.get_projection_matrix() * camera.get_view_matrix() * translate_m);
            p_light_shader_program->setVec3("light_color", glm::vec3(light_color[0], light_color[1], light_color[2]));
            Renderer_OpenGL::draw(*p_cube_vao);
        }

        //----------------------------------//
        // ImGui отрисовка интерфейса

        UIModule::on_ui_draw_begin();

        on_ui_draw();
        UIModule::on_ui_draw_end();

        m_pWindow->on_update();
        on_update();
    }

    int Application::start(unsigned int window_width, unsigned int window_height, const char* title)
    {
        // Создание окна
        m_pWindow = std::make_unique<Window>(title, window_width, window_height);
        // Изменение камеры под размеры окна
        camera.set_viewport_size(static_cast<float>(window_width), static_cast<float>(window_height));

        // Подписание на событие перемещения курсора
        m_event_dispatcher.add_event_listener<EventMouseMoved>(
            [](EventMouseMoved& event)
            {
                //LOG_INFO("[MouseMoved] Mouse moved to x {0}; y {1}", event.x, event.y);
            }
        );

        // Подписание на событие изменения размеров окна
        m_event_dispatcher.add_event_listener<EventWindowResize>(
            [&](EventWindowResize& event)
            {
                LOG_INFO("[WindowResize] Window resized to {0}x{1}", event.width, event.height);
                camera.set_viewport_size(event.width, event.height);
            }
        );

        // Подписание на событие закрытия окна
        m_event_dispatcher.add_event_listener<EventWindowClose>(
            [&](EventWindowClose& event)
            {
                LOG_INFO("[WindowClose]");
                close();
            }
        );

        // Подписание на событие нажатия клавиши клавиатуры
        m_event_dispatcher.add_event_listener<EventKeyPressed>(
            [&](EventKeyPressed& event)
            {
                LOG_INFO("[EventKeyPressed] {0}", static_cast<char>(event.key_code));

                if (event.key_code <= KeyCode::KEY_Z)
                {
                    if (event.repeated)
                    {
                        LOG_INFO("\t[EventKeyPressed] [Repeated] {0}", static_cast<char>(event.key_code));
                    } else
                    {
                        LOG_INFO("\t[EventKeyPressed] {0}", static_cast<char>(event.key_code));
                    }
                }
                
                Input::PressKey(event.key_code);
            }
        );

        // Подписание на событие отжатия клавиши клавиатуры
        m_event_dispatcher.add_event_listener<EventKeyReleased>(
            [&](EventKeyReleased& event)
            {
                LOG_INFO("[EventKeyReleased] {0}", static_cast<char>(event.key_code));
                Input::ReleasedKey(event.key_code);
            }
        );

        // Подписание на событие нажатия кнопки мыши
        m_event_dispatcher.add_event_listener<EventMouseButtonPressed>(
            [&](EventMouseButtonPressed& event)
            {
                LOG_INFO("[EventMouseButtonPressed] {0}", static_cast<int>(event.mouse_button_code));
                LOG_INFO("\t[EventMouseButtonPressed] At ({0}, {1})", event.x_pos, event.y_pos);
                
                Input::PressMouseButton(event.mouse_button_code);
                on_mouse_button_pressed_event(event.mouse_button_code, event.x_pos, event.y_pos, true);
            }
        );

        // Подписание на событие отжатия кнопки мыши
        m_event_dispatcher.add_event_listener<EventMouseButtonRealeased>(
            [&](EventMouseButtonRealeased& event)
            {
                LOG_INFO("[EventMouseButtonRealeased] {0}", static_cast<int>(event.mouse_button_code));
                LOG_INFO("\t[EventMouseButtonRealeased] At ({0}, {1})", event.x_pos, event.y_pos);
                
                Input::ReleaseMouseButton(event.mouse_button_code);
                on_mouse_button_pressed_event(event.mouse_button_code, event.x_pos, event.y_pos, false);
            }
        );

        // Передача события окну
        m_pWindow->set_event_callback(
            [&](BaseEvent& event)
            {
                m_event_dispatcher.dispatch(event);
            }
        );

        // Определение позиций 3D объектов
        positions = {
            glm::vec3(-2.f, -2.f, -4.f),
            glm::vec3(-5.f, 0.f, 3.f),
            glm::vec3(2.f, 1.f, -2.f),
            glm::vec3(4.f, -3.f, 3.f),
            glm::vec3(1.f, -7.f, 1.f)
        };

        // Создание текстуры
        const unsigned int width = 1000;
        const unsigned int height = 1000;
        const unsigned int channels = 3;
        auto* data = new unsigned char[width * height * channels];

        generate_quads_texture(data, width, height);
        p_texture_none = std::make_unique<Texture2D>(data, width, height);
        p_texture_none->bind(0);

        delete[] data;
        
        // Создание объекта класса шейдерной программы для объекта
        p_shader_program = std::make_unique<ShaderProgram3D>(vertex_shader, fragment_shader);
        if (!p_shader_program->isComplited()) return false;

        // Создание "оболочки" буффера
        /*
        В переменной cube_pos_norm_uv описываются следующие элементы:
            1. координаты вершины из трех переменных типа float;
            2. координаты нормали вершины из трех переменных float;
            3. координаты двухмерной текстуры из двух переменных float.
        */ 
        BufferLayout buffer_layout_vec3_vec3_vec2
        {
            ShaderDataType::Float3,
            ShaderDataType::Float3,
            ShaderDataType::Float2
        };

        // Создание объекта класса массива вершин
        p_cube_vao = std::make_unique<VertexArray>();
        // Создание объекта класса вершинного буффера
        p_cube_positions_vbo = std::make_unique<VertexBuffer>(cube_pos_norm_uv, sizeof(cube_pos_norm_uv), buffer_layout_vec3_vec3_vec2);
        // Создание объекта класса индексного буффера
        p_cube_index_buffer = std::make_unique<IndexBuffer>(indexes, sizeof(indexes) / sizeof(GLuint));
        // Указание вершинного и индексных буфферов в массиве вершин
        p_cube_vao->add_vertex_buffer(*p_cube_positions_vbo);
        p_cube_vao->set_index_buffer(*p_cube_index_buffer);

        // Создание объекта класса шейдерной программы для освещения
        p_light_shader_program = std::make_unique<ShaderProgram3D>(light_vertex_shader, light_fragment_shader);
        if (!p_light_shader_program->isComplited()) return false;

        // Определение буффера глубины
        Renderer_OpenGL::enable_depth_testing();

        // Главный цикл фреймворка, цикл рендеринга
        while (!m_bCloseWindow)
        {
            draw();
        }
        m_pWindow = nullptr;

        return 0;
    }

    void Application::close()
    {
        m_bCloseWindow = true;
    }

    glm::vec2 Application::get_current_cursor_position() const
    {
        return m_pWindow->get_current_cursor_position();
    }

    void Application::add_new_position(glm::vec3 position)
    {
        positions.push_back(position);
    }
}