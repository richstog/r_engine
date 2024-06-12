#pragma once

#include <r_engine_core/Keys.hpp>
#include <stddef.h>

// Определение статического класса входных данных
// Описание класса в файле Input.cpp
namespace r_engine
{
    class Input
    {
        public:
        // Статические методы для реализации нажатия клавиш клавиатуры
        static bool IsKeyPressed(const KeyCode key_code);
        static void PressKey(const KeyCode key_code);
        static void ReleasedKey(const KeyCode key_code);

        // Статические метода для реализации нажатия кнопок мыши
        static bool isMouseButtonPressed(const MouseButtonCode mouse_button);
        static void PressMouseButton(const MouseButtonCode mouse_button);
        static void ReleaseMouseButton(const MouseButtonCode mouse_button);

        private:
        // Статические поля, массивы нажатых клавиш и кнопок
        static bool m_keys_pressed[];
        static bool m_mouse_buttons_pressed[];
    };
}