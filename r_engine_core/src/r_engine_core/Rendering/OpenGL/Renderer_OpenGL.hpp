#pragma once

struct GLFWwindow;

// Определение статического класса рендера OpenGL
// Описание класса в файле Renderer_OpenGL.cpp
namespace r_engine
{
    class VertexArray;

    class Renderer_OpenGL
    {
        public:
        // Метод инициализации OpenGL
        static bool init(GLFWwindow* pWindow);

        // Метод отрисовки (рендера) массива вершин
        static void draw(const VertexArray& vertex_array);
        // Метод определения цвета по default
        static void set_clear_color(const float r, const float g, const float b, const float a);
        // Метод очистки буффера кадра
        static void clear();
        // Метод определения области отрисовки (рендера)
        static void set_viewport(
            const unsigned int width,
            const unsigned int height,
            const unsigned int left_offset = 0,
            const unsigned int bottom_offset = 0
        );
        // Метод определяющий конструирования буффера глубины
        static void enable_depth_testing();
        // Метод определяюший деконструирование буффера глубины
        static void disable_depth_testing();

        // Метод получения информации об устройстве, на котором производится рендер
        static const char* get_vendor_str();
        // Метод получения информации о рендеринге
        static const char* get_renderer_str();
        // Метод получения информации о версии OpenGL
        static const char* get_version_str();
    };
}
