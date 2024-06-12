#pragma once

struct GLFWwindow;

// Определение статического класса инициализации пользовательского интерфейса IMGUI
// Описание класса в файле UIModule.cpp
namespace r_engine
{
    class UIModule
    {
        public:
        // Метод инициализации контекста IMGUI при создании окна
        static void on_window_create(GLFWwindow* pWindow);
        // Метод удаления контекста IMGUI при закрытии окна
        static void on_window_close();
        // Метод определяющий начало отрисовки графического интерфейса
        static void on_ui_draw_begin();
        // Метод определяющий окончание отрисовки графического интерфейса
        static void on_ui_draw_end();
    };
}