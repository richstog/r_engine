#include "ShaderProgram.hpp"

#include <r_engine_core/Log.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace r_engine
{
    bool create_shader(const char* source, const GLenum shader_type, GLuint &shader_id)
    {
        shader_id = glCreateShader(shader_type);
        glShaderSource(shader_id, 1, &source, nullptr);
        glCompileShader(shader_id);

        GLint success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            char info_log[1024];
            glGetShaderInfoLog(shader_id, 1024, nullptr, info_log);

            std::string a(info_log);
            LOG_CRIT("[Shader Error]\n{}", info_log);
            return false;
        }
        return true;
    }

    ShaderProgram::ShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src)
    {
        GLuint vertex_shader_id = 0;
        if (!create_shader(vertex_shader_src, GL_VERTEX_SHADER, vertex_shader_id))
        {
            LOG_CRIT("[VERTEX SHADER] Compile-time error");
            glDeleteShader(vertex_shader_id);
            return;
        }

        GLuint fragment_shader_id = 0;
        if (!create_shader(fragment_shader_src, GL_FRAGMENT_SHADER, fragment_shader_id))
        {
            LOG_CRIT("[FRAGMENT SHADER] Compile-time error");
            glDeleteShader(vertex_shader_id);
            glDeleteShader(fragment_shader_id);
            return;
        }

        m_id = glCreateProgram();
        glAttachShader(m_id, vertex_shader_id);
        glAttachShader(m_id, fragment_shader_id);
        glLinkProgram(m_id);

        GLint success;
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLchar info_log[1024];
            glGetProgramInfoLog(m_id, 1024, nullptr, info_log);
            LOG_CRIT("[SHADER PROGRAM] Link-time error");
            glDeleteProgram(m_id);
            m_id = 0;
            glDeleteShader(vertex_shader_id);
            glDeleteShader(fragment_shader_id);
            return;
        }
        else
        {
            m_isCompiled = true;
        }

        glDetachShader(m_id, vertex_shader_id);
        glDetachShader(m_id, fragment_shader_id);

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_id);
    }

    void ShaderProgram::bind() const
    {
        glUseProgram(m_id);
    }

    void ShaderProgram::unbind()
    {
        glUseProgram(0);
    }

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram)
    {
        glDeleteProgram(m_id);
        m_id = shaderProgram.m_id;
        m_isCompiled = shaderProgram.m_isCompiled;

        shaderProgram.m_id = 0;
        shaderProgram.m_isCompiled = false;
        return *this;
    }

    ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram)
    {
        m_id = shaderProgram.m_id;
        m_isCompiled = shaderProgram.m_isCompiled;

        shaderProgram.m_id = 0;
        shaderProgram.m_isCompiled = false;
    }

    void ShaderProgram::setMatrix3(const char* name, const glm::mat3& value) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setMatrix4(const char* name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setInt(const char* name, const int value) const
    {
        glUniform1i(glGetUniformLocation(m_id, name), value);
    }

    void ShaderProgram::setVec3(const char* name, const glm::vec3& value) const
    {
        glUniform3f(glGetUniformLocation(m_id, name), value.x, value.y, value.z);
    }

    void ShaderProgram::setFloat(const char* name, const float value) const
    {
        glUniform1f(glGetUniformLocation(m_id, name), value);
    }
}