#include <r_engine_core/Input.hpp>

namespace r_engine
{
    // Определение массива клавиш клавиатуры
    bool Input::m_keys_pressed[static_cast<size_t>(KeyCode::KEY_LAST) + 1] = {};
    // Определение массива кажатых кнопок мыши
    bool Input::m_mouse_buttons_pressed[static_cast<size_t>(MouseButtonCode::MOUSE_BUTTON_LAST) + 1] = {};

    // Определение методов нажатия и отжатия клавиш и кнопок
    bool Input::IsKeyPressed(const KeyCode key_code)
    {
        return m_keys_pressed[static_cast<size_t>(key_code)];
    }

    void Input::PressKey(const KeyCode key_code)
    {
        m_keys_pressed[static_cast<size_t>(key_code)] = true;
    }

    void Input::ReleasedKey(const KeyCode key_code)
    {
        m_keys_pressed[static_cast<size_t>(key_code)] = false;
    }

    bool Input::isMouseButtonPressed(const MouseButtonCode mouse_button)
    {
        return m_mouse_buttons_pressed[static_cast<size_t>(mouse_button)];
    }

    void Input::PressMouseButton(const MouseButtonCode mouse_button)
    {
        m_mouse_buttons_pressed[static_cast<size_t>(mouse_button)] = true;
    }

    void Input::ReleaseMouseButton(const MouseButtonCode mouse_button)
    {
        m_mouse_buttons_pressed[static_cast<size_t>(mouse_button)] = false;
    }
}