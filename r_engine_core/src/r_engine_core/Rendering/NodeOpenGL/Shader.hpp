#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>

#include <map>
#include <string>

namespace r_engine
{
    // Класс шейдерной программы
    class ShaderProgram
    {
        public:
            ShaderProgram();
            ShaderProgram(const ShaderProgram &copy);
            ~ShaderProgram();
            ShaderProgram& operator=(const ShaderProgram& other);

            void use(); // Использование шейдеров
            void load(GLuint type, const char* filename); // Функция для загрузки шейдеров
            void link(); // Формирование программы из загруженных шейдеров
            GLuint getUniformLoc(const char* name); // Возвращает местоположение uniform-переменной
            void bindUniformBlock(const char* name, int binding); // Привязка uniform-блока
            void bindTextures(const char* textures_base_shader_names[], int count); // Инициализация текстур на шейдере
        private:
            GLuint program; // Дескриптор
            static std::map<int, int> handler_count; // Получение количества использований по дескриптору шейдера (Shared pointer)
            std::map<const char*, GLuint> uniformLocations; // Местоположения uniform-переменных
    };
}