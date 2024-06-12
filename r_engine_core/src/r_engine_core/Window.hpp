#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <functional>
#include <glm/ext/vector_float2.hpp>

#include "r_engine_core/Event.hpp"

// Определение класса окна
// Описание класса в файле Window.cpp
namespace r_engine {

    class Window {

        public:
        // Использование функции колл-бека для переопределения событий окна
        using EventCallbackFunc = std::function<void(BaseEvent&)>;

        // Конструктор с параметрами и деструктор
        Window(std::string title, const unsigned int width, const unsigned height);
        ~Window();

        // Запрет на копирование и перемещение
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        // Метод обновления кадра
        void on_update();

        // Геттера
        unsigned int get_width() const { return m_data.width; };
        unsigned int get_height() const { return m_data.height; };
        glm::vec2 get_current_cursor_position() const;

        // Сеттер колл-бека событий
        void set_event_callback(const EventCallbackFunc& callback)
        {
            m_data.eventCallBackFunc = callback;
        }

        private:

        // Структура данных окна для переопределения событий
        struct WindowData
        {
            std::string title;
            unsigned int width;
            unsigned int height;
            EventCallbackFunc eventCallBackFunc;
        };

        // Метод инициализации GLFW
        int init();
        // Метод завершения работы окна
        void shutdown();

        // Указатель на объект окна
        GLFWwindow* m_pWindow = nullptr;
        // Данные окна
        WindowData m_data;
    };
}