#include <iostream>
#include <memory>

#include <r_engine_core/Application.hpp>
#include <r_engine_core/Log.hpp>
#include <r_engine_core/Input.hpp>
#include <r_engine_core/Camera3D.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <string>
#include <format>
#include <sstream>

/*
    РЕДАКТОР ДВИЖКА
        В этом файле предоставлен код использования фреймворка 3D движка.
        1. Создание класса, унаследованного от класса Application;
        2. Переопределение методов класса Application:
            а) on_update;
            б) on_mouse_button_pressed_event;
            в) on_ui_draw.
        3. Создание объекта, созданного ранее класса;
        4. Определить размеры окна и наименование окна
    
    ПРИМЕЧАНИЯ
        1. Нельзя создать несколько приложений, во избежание ошибок с памятью необходимо использовать умные указатели, к примеру: make_unique;
        2. В проекте используется namespace под наименованием r_engine, только через него можно получить доступ к классам движка.

    ДЛЯ ОПЫТНЫХ ПОЛЬЗОВАТЕЛЕЙ
        Если вы опытный пользователь и хотите использовать классы рендеринга, необходимо изменить настройку CMake сборки.
        Инструкция для опытных пользователей, которые желают использовать движок и классы рендеринга:
            1. Перейти в CMake файл движка, находящйися в папке "r_engine_core/CMakeLists.txt";
            2. Изменить в 75 строке привязку PRIVATE на PUBLIC;
            3. Если появились ошибки, то необходимо подключить библиотеки для разработки в редактор:
                а) перейти в CMake файл редактора, находящийся в папке "r_engine_editor/CMakeLists.txt";
                б) дописать необходимые библиотеки в 10 строке (библиотеки описаны в 9 строке).
*/

// Переопределение класса приложения Application
// Переопределение методов класса приложения Application
class R_Engine_Editor : public r_engine::Application {

    double m_initial_mouse_pos_x = 0.0;
    double m_initial_mouse_pos_y = 0.0;

    float camera_position[3] = {0.f, 0.f, 1.f};
    float camera_rotation[3] = {0.f, 0.f, 0.f};
    float camera_fov = 60.f;
    float camera_near_plane = 0.1f;
    float camera_far_plane = 100.f;
    bool camera_perspective = true;

    virtual void on_update() override
    {
        glm::vec3 movement_delta {0, 0, 0};
        glm::vec3 rotation_delta {0, 0, 0};

        // Перемещение камеры
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_W))
        {
            movement_delta.x += 0.05f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_S))
        {
            movement_delta.x -= 0.05f;
        }

        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_A))
        {
            movement_delta.y -= 0.05f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_D))
        {
            movement_delta.y += 0.05f;
        }

        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_E))
        {
            movement_delta.z += 0.05f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_Q))
        {
            movement_delta.z -= 0.05f;
        }

        // Поворот камеры
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_UP))
        {
            rotation_delta.y -= 0.5f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_DOWN))
        {
            rotation_delta.y += 0.5f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_LEFT))
        {
            rotation_delta.z += 0.5f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_RIGHT))
        {
            rotation_delta.z -= 0.5f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_P))
        {
            rotation_delta.x += 0.5f;
        }
        if (r_engine::Input::IsKeyPressed(r_engine::KeyCode::KEY_O))
        {
            rotation_delta.x -= 0.5f;
        }

        if (r_engine::Input::isMouseButtonPressed(r_engine::MouseButtonCode::MOUSE_BUTTON_RIGHT))
        {
            glm::vec2 current_cursor_position = get_current_cursor_position();
            if (r_engine::Input::isMouseButtonPressed(r_engine::MouseButtonCode::MOUSE_BUTTON_LEFT))
            {
                camera.move_right(static_cast<float>((current_cursor_position.x - m_initial_mouse_pos_x) / 100.f));
                camera.move_up(static_cast<float>((m_initial_mouse_pos_y - current_cursor_position.y) / 100.f));
            } else
            {
                rotation_delta.z += static_cast<float>((m_initial_mouse_pos_x - current_cursor_position.x) / 5.f);
                rotation_delta.y -= static_cast<float>((m_initial_mouse_pos_y - current_cursor_position.y) / 5.f);
            }

            m_initial_mouse_pos_x = current_cursor_position.x;
            m_initial_mouse_pos_y = current_cursor_position.y;
        }

        camera.add_movement_and_rotation(movement_delta, rotation_delta);
    }

    // Метод конструирования интерфейса IMGUI
    void setup_dockspace_menu()
    {
        /* Dockspace flags */
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoWindowMenuButton;
        dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;

        /* Window flags */
        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        /* Opt fullscreen */
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        /* Opt padding */
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Dockspace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        /* Create dockspace */
        ImGuiIO& io = ImGui::GetIO();
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        /* Create menubar */
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Import obj file", NULL))
                {

                }

                if (ImGui::MenuItem("Export obj file", NULL))
                {

                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("Create scene", NULL))
                {

                }

                if (ImGui::MenuItem("Import scene", NULL))
                {

                }

                if (ImGui::MenuItem("Export scene", NULL))
                {

                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Light"))
            {
                if (ImGui::MenuItem("Create light", NULL))
                {

                }

                if (ImGui::MenuItem("List light", NULL))
                {

                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings"))
            {
                if (ImGui::MenuItem("Exit", NULL))
                {
                    close();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    virtual void on_mouse_button_pressed_event(const r_engine::MouseButtonCode button_code, const double x_pos, const double y_pos, const bool pressed) override
    {
        m_initial_mouse_pos_x = x_pos;
        m_initial_mouse_pos_y = y_pos;
    }

    virtual void on_ui_draw() override
    {
        setup_dockspace_menu();

        camera_position[0] = camera.get_position().x;
        camera_position[1] = camera.get_position().y;
        camera_position[2] = camera.get_position().z;

        camera_rotation[0] = camera.get_rotation().x;
        camera_rotation[1] = camera.get_rotation().y;
        camera_rotation[2] = camera.get_rotation().z;

        camera_fov = camera.get_field_of_view();
        camera_far_plane = camera.get_far_clip_plane();
        camera_near_plane = camera.get_near_clip_plane();

        /* CAMERA UI */
        ImGui::Begin("Camera");
            if (ImGui::SliderFloat3("Position", camera_position, -10.f, 10.f))
            {
                camera.set_position(glm::vec3(camera_position[0], camera_position[1], camera_position[2]));
            }
            if (ImGui::SliderFloat3("Rotate", camera_rotation, 0.f, 360.f))
            {
                camera.set_rotation(glm::vec3(camera_rotation[0], camera_rotation[1], camera_rotation[2]));
            }
            if (ImGui::SliderFloat("FOV", &camera_fov, 1.f, 120.f)) // Ширина обзора
            {
                camera.set_field_of_view(camera_fov);
            }
            if (ImGui::SliderFloat("Near clip plane", &camera_near_plane, 0.1f, 10.f)) // Ближний край
            {
                camera.set_near_clip_plane(camera_near_plane);
            }
            if (ImGui::SliderFloat("Far clip plane", &camera_far_plane, 1.f, 100.f)) // Дальний край
            {
                camera.set_far_clip_plane(camera_far_plane);
            }
            if (ImGui::Checkbox("Perspective", &camera_perspective))
            {
                camera.set_projection_mode(
                    camera_perspective ?
                    r_engine::Camera3D::ProjectionMode::Perspective : r_engine::Camera3D::ProjectionMode::Orthographic
                );
            }
        ImGui::End();

        /* LIGHT UI */
        ImGui::Begin("Light");
            ImGui::SliderFloat3("Position", light_position, -10.f, 10.f);
            ImGui::ColorEdit3("Color", light_color);
            ImGui::SliderFloat("Ambient factor", &light_ambient_factor, 0.f, 1.f); // Фоновое
            ImGui::SliderFloat("Diffuse factor", &light_diffuse_factor, 0.f, 1.f); // Диффузия
            ImGui::SliderFloat("Specular factor", &light_specular_factor, 0.f, 1.f); // Отражение
            ImGui::SliderFloat("Shininess", &light_shininess, 1.f, 128.f); // Блик
        ImGui::End();

        ImGui::Begin("Models");

            ImGui::SliderFloat3("Position", new_model_position, -100.f, 100.f);
            if (ImGui::Button("Create"))
            {
                add_new_position(
                    glm::vec3(new_model_position[0], new_model_position[1], new_model_position[2])
                );
            }

            for (int i = 0; i < positions.size(); i++)
            {
                std::stringstream ss;
                ss << "Position x:" << positions[i].x << " y:" << positions[i].y << " z:" << positions[i].z;
                ImGui::LabelText(ss.str().c_str(), std::to_string(i+1).c_str());
            }

        ImGui::End();
    }

    int frame = 0;
};

int main()
{
    // Создание объекта приложения
    std::unique_ptr r_engine_editor = std::make_unique<R_Engine_Editor>();

    // Результат создания приложения
    int returnCode = r_engine_editor->start(1024, 768, "R Engine Editor");

    return returnCode;
}
