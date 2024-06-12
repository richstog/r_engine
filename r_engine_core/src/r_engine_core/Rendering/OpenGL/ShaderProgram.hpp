#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

namespace r_engine
{
    class ShaderProgram
    {
        public:
        ShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src);
        ShaderProgram(ShaderProgram&&);
        ShaderProgram& operator=(ShaderProgram&&);
        ~ShaderProgram();

        ShaderProgram() = delete;
        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;

        void bind() const;
        static void unbind();
        bool isComplited() const { return m_isCompiled; }
        void setMatrix3(const char* name, const glm::mat3& value) const;
        void setMatrix4(const char* name, const glm::mat4& value) const;
        void setInt(const char* name, const int value) const;
        void setFloat(const char* name, const float value) const;
        void setVec3(const char* name, const glm::vec3& value) const;

        private:
        bool m_isCompiled = false;
        GLuint m_id = 0;
    };
}