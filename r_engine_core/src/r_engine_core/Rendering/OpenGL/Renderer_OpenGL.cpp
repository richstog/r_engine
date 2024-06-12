#include <r_engine_core/Rendering/OpenGL/Renderer_OpenGL.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <r_engine_core/Rendering/OpenGL/VertexArray.hpp>
#include <r_engine_core/Log.hpp>

namespace r_engine
{
    // Инициализация контекста OpenGL в окне GLFW
    bool Renderer_OpenGL::init(GLFWwindow* pWindow)
    {
        glfwMakeContextCurrent(pWindow);

        // Загрузка функций OpenGL через GLAD
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            LOG_CRIT("Failed to initialize GLAD");
            return false;
        }

        // Вывод информации об OpenGL
        LOG_INFO("OpenGL context initialized:");
        LOG_INFO("\tVendor {0}", get_vendor_str());
        LOG_INFO("\tRenderer {0}", get_renderer_str());
        LOG_INFO("\tVersion {0}", get_version_str());
        
        return true;
    }

    // Отрисовка массива вершин функцией glDrawElements
    void Renderer_OpenGL::draw(const VertexArray& vertex_array)
    {
        vertex_array.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_array.get_indexes_count()), GL_UNSIGNED_INT, nullptr);
    }

    // Определение цвеа буфера очистки
    void Renderer_OpenGL::set_clear_color(const float r, const float g, const float b, const float a)
    {
        glClearColor(r, g, b, a);
    }

    // Очистка буфера рендеринга
    void Renderer_OpenGL::clear()
    {
        // Вначале определение цвета из буфера очистки, потом удаление вершин глубины
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Определение области отрисовки
    void Renderer_OpenGL::set_viewport(
        const unsigned int width,
        const unsigned int height,
        const unsigned int left_offset,
        const unsigned int bottom_offset
    )
    {
        glViewport(left_offset, bottom_offset, width, height);
    }

    // Разрешение глубинного буффера
    void Renderer_OpenGL::enable_depth_testing()
    {
        glEnable(GL_DEPTH_TEST);
    }

    // Отключение глубинного буффера
    void Renderer_OpenGL::disable_depth_testing()
    {
        glDisable(GL_DEPTH_TEST);
    }

    // Получение информации об устройстве, на котором производится рендер
    const char* Renderer_OpenGL::get_vendor_str()
    {
        return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    }

    // Получение информации о рендеринге
    const char* Renderer_OpenGL::get_renderer_str()
    {
        return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    }

    // Получение информации о версии OpenGL
    const char* Renderer_OpenGL::get_version_str()
    {
        return reinterpret_cast<const char*>(glGetString(GL_VERSION));
    }
}