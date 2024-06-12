#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "r_engine_core/Log.hpp"
#include "r_engine_core/Event.hpp"

namespace r_engine
{
    class NodeApplication
    {
        public:
        NodeApplication();
        virtual ~NodeApplication();

        NodeApplication(const NodeApplication&) = delete;
        NodeApplication(NodeApplication&&) = delete;

        NodeApplication& operator=(const NodeApplication&) = delete;
        NodeApplication& operator=(NodeApplication&&) = delete;

        virtual int start(unsigned int window_width, unsigned int window_height, const char* title);
        void close();
        virtual void on_update() {}
        virtual void on_ui_draw() {}
        virtual void on_mouse_button_pressed_event(const MouseButtonCode button_code, const double x_pos, const double y_pos, const bool pressed) {}

        glm::vec2 get_current_cursor_position() const;

        private:
        void draw();
        std::unique_ptr<class Window> m_pWindow;

        float inv_gamma = 1 / 2.2;


        EventDispatcher m_event_dispatcher;
        bool m_bCloseWindow = false;
    };
}