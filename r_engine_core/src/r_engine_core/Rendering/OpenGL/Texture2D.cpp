#include <r_engine_core/Rendering/OpenGL/Texture2D.hpp>

#include <glad/glad.h>
#include <glm/trigonometric.hpp>

namespace r_engine
{
    // Конструктор, определяющий текстуру в памяти видеокарты
    Texture2D::Texture2D(const unsigned char* data, const unsigned int width, const unsigned int height)
    : m_width(width), m_height(height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        const GLsizei mip_levels = static_cast<GLsizei>(log2(std::max(m_width, m_height))) + 1;
        glTextureStorage2D(m_id, mip_levels, GL_RGB8, m_width, m_height);
        glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, data);

        // Определение параметров повторения текстуры
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Определение параметров фильтрации текстуры
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Создание mipmap текстуры
        glGenerateMipmap(m_id);
    }

    // Удаление текстуры из видеопамяти
    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &m_id);
    }

    // Перемещение текстуры через оператор равенства с правосторонним значением
    Texture2D& Texture2D::operator= (Texture2D&& texture) noexcept
    {
        glDeleteTextures(2, &m_id);
        m_id = texture.m_id;
        m_width = texture.m_width;
        m_height = texture.m_height;
        texture.m_id = 0;
        return *this;
    }

    // Перемещение текстуры через конструктор с правосторонним значением
    Texture2D::Texture2D(Texture2D&& texture) noexcept
    {
        m_id = texture.m_id;
        m_width = texture.m_width;
        m_height = texture.m_height;
        texture.m_id = 0;
    }

    // Определение работы с данной текстурой
    void Texture2D::bind(const unsigned int unit) const
    {
        glBindTextureUnit(unit, m_id);
    }
}