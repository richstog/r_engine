#define GLFW_INCLUDE_NONE

#include <iostream>

#include <r_engine_core/Window.hpp>
#include <r_engine_core/Log.hpp>

#include <r_engine_core/Rendering/OpenGL/Renderer_OpenGL.hpp>
#include <r_engine_core/Modules/UIModule.hpp>

#include "Rendering/OpenGL/Renderer_OpenGL.hpp"

namespace r_engine
{
    // Конструктор окна с параметрами
    Window::Window(std::string title, const unsigned int width, const unsigned height)
        : m_data({std::move(title), width, height})
    {
        int result = init();
    }

    // Деструктор окна с параметрами
    Window::~Window()
    {
        shutdown();
    }


    // Инициализация окна
    int Window::init()
    {
        LOG_INFO("Creating window '{0}' {1}x{2}", m_data.title, m_data.width, m_data.height);

        // Переопределение события, если контекст GLFW не будет создан
        glfwSetErrorCallback([](int error_code, const char* description)
        {
            LOG_CRIT("GLFW error: {0}", description);
        });

        // Инициализация контекста GLFW
        if (!glfwInit())
        {
            LOG_CRIT("GLFW is not initialized");
            return -1;
        }
        
        // Создание окна
        LOG_INFO("Create window");
        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
        if (!m_pWindow)
        {
            LOG_CRIT("Window {0} was not created", m_data.title);
            return -2;
        }

        // Создание контекста OpenGL в окне GLFW
        if (!Renderer_OpenGL::init(m_pWindow))
        {
            LOG_CRIT("Failed Renderer_OpenGL::init");
            return -3;
        }
        
        // Определение данных окна для пользовательских переопределений событий
        glfwSetWindowUserPointer(m_pWindow, &m_data);

        // Переопределение колл-бека нажатия клавиши клавиатуры
        glfwSetKeyCallback(m_pWindow,
            [](GLFWwindow* pWindow, int key, int scancode, int action, int mods)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                switch (action)
                {
                    case GLFW_PRESS:
                    {
                        EventKeyPressed event(static_cast<KeyCode>(key), false);
                        data.eventCallBackFunc(event);
                        break;
                    }
                        
                    case GLFW_RELEASE:
                    {
                        EventKeyReleased event(static_cast<KeyCode>(key));
                        data.eventCallBackFunc(event);
                        break;
                    }

                    case GLFW_REPEAT:
                    {
                        EventKeyPressed event(static_cast<KeyCode>(key), true);
                        data.eventCallBackFunc(event);
                        break;
                    }
                }
            }
        );

        // Переопределение колл-бека нажатия кнопки мышки
        glfwSetMouseButtonCallback(m_pWindow,
            [](GLFWwindow* pWindow, int button, int action, int mods)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                double x_pos; double y_pos;
                glfwGetCursorPos(pWindow, &x_pos, &y_pos);
                switch (action)
                {
                    case GLFW_PRESS:
                    {
                        EventMouseButtonPressed event(static_cast<MouseButtonCode>(button), x_pos, y_pos);
                        data.eventCallBackFunc(event);
                        break;
                    }

                    case GLFW_RELEASE:
                    {
                        EventMouseButtonRealeased event(static_cast<MouseButtonCode>(button), x_pos, y_pos);
                        data.eventCallBackFunc(event);
                        break;
                    }
                }
            }
        );

        // Переопределение колл-бека изменения размеров окна
        glfwSetWindowSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                data.width = width;
                data.height = height;
                EventWindowResize event(width, height);
                data.eventCallBackFunc(event);
            }
        );

        // Переопределение колл-бека движения курсора в окне
        glfwSetCursorPosCallback(m_pWindow,
            [](GLFWwindow* pWindow, double x, double y)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                EventMouseMoved event(x, y);
                data.eventCallBackFunc(event);
            }
        );

        // Переопределение колл-бека закрытия окна
        glfwSetWindowCloseCallback(m_pWindow,
            [](GLFWwindow* pWindow)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                EventWindowClose event;
                data.eventCallBackFunc(event);
            }
        );

        // Переопределение колл-бека изменения буффера рендеринга
        glfwSetFramebufferSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height)
            {
                Renderer_OpenGL::set_viewport(width, height, 0, 0);
            }
        );

        // Создание контекста IMGUI в окне GLFW
        UIModule::on_window_create(m_pWindow);

        return 0;
    }

    // Завершение работы окна
    void Window::shutdown()
    {
        UIModule::on_window_close();
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }

    // Покадровые изменения
    void Window::on_update()
    {
        // Перемещение буфферов
        glfwSwapBuffers(m_pWindow);
        // Отслеживание событий
        glfwPollEvents();
    }

    // Получение позиции курсора в окне
    glm::vec2 Window::get_current_cursor_position() const
    {
        double x_pos;
        double y_pos;
        glfwGetCursorPos(m_pWindow, &x_pos, &y_pos);
        return {x_pos, y_pos};
    }
}