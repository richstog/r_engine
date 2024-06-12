#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

// Определение класса шейдерной программыы
// Описание класса в файле ShaderProgram3D.cpp
namespace r_engine
{
    class ShaderProgram3D
    {
        public:
        ShaderProgram3D(const char* vertex_shader_src, const char* fragment_shader_src);
        ShaderProgram3D(ShaderProgram3D&&);
        ShaderProgram3D& operator=(ShaderProgram3D&&);
        ~ShaderProgram3D();

        // Удаление конструктора без параметров
        ShaderProgram3D() = delete;
        // Удаление конструкторов копирования
        ShaderProgram3D(const ShaderProgram3D&) = delete;
        ShaderProgram3D& operator=(const ShaderProgram3D&) = delete;

        // Функции для работы с шейдерной программой
        void bind() const;
        static void unbind();
        // Функция успешной компиляции и линковки программы
        bool isComplited() const { return m_isCompiled; }
        // Функции добавления данных в программу, для дальнейшей работы с ними в шейдерах
        void setMatrix3(const char* name, const glm::mat3& value) const;
        void setMatrix4(const char* name, const glm::mat4& value) const;
        void setInt(const char* name, const int value) const;
        void setFloat(const char* name, const float value) const;
        void setVec3(const char* name, const glm::vec3& value) const;

        private:
        // Успешная компиляция и линковка?
        bool m_isCompiled = false;
        // Номер программы, выданный OpenGL
        GLuint m_id = 0;
    };
}
