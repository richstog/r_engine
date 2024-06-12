#pragma once

#include <memory>

#include "r_engine_core/Log.hpp"
#include "r_engine_core/Event.hpp"
#include "r_engine_core/Camera3D.hpp"

// Определение класса приложения
// Описание класса в файле Application.cpp
namespace r_engine
{

    class Application
    {
        public:
        // Конструктор класса без параметров
        Application();

        // Виртуальный деструктор
        virtual ~Application();

        // Удаление конструкторов копирования
        Application(const Application&) = delete;
        Application(Application&&) = delete;

        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        // Виртуальный метод начала работы приложения
        virtual int start(unsigned int window_width, unsigned int window_height, const char* title);
        // Метод завершения приложения
        void close();
        // Виртуальные пользовательские методы для редактора
        virtual void on_update() {}
        virtual void on_ui_draw() {}
        virtual void on_mouse_button_pressed_event(const MouseButtonCode button_code, const double x_pos, const double y_pos, const bool pressed) {}

        // Получение координат курсора мыши
        glm::vec2 get_current_cursor_position() const;

        // Публичные поля класса приложения
        Camera3D camera{ glm::vec3(-5, 0, 0) };

        std::vector<glm::vec3> positions;
        float new_model_position[3] = { 0.f, 0.f, 0.f };

        float light_position[3] = { 0.f, 0.f, 0.f };
        float light_color[3] = { 1.f, 1.f, 1.f };
        float light_ambient_factor = 0.1f;
        float light_diffuse_factor = 1.f;
        float light_specular_factor = 0.5f;
        float light_shininess = 32.f;

        // Создание новой позиции объекта
        void add_new_position(glm::vec3 position);

        private:

        // Метод отрисовки (рендер)
        void draw();

        // Приватные поля класса приложения
        std::unique_ptr<class Window> m_pWindow;

        EventDispatcher m_event_dispatcher;
        bool m_bCloseWindow = false;
    };
}