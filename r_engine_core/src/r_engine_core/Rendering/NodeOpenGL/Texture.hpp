#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>

#include <map>
#include <string>

namespace r_engine
{
    enum TexType {
        TEX_DIFFUSE,
        TEX_AMBIENT,
        TEX_SPECULAR,
        TEX_AVAILABLE_COUNT
    };

    // Абстрактный класс базовой текстуры
    class BaseTexture
    {
        public:
            ~BaseTexture();
            virtual void use() = 0; // Привязка текстуры
            static void disable(GLuint type); // Отвязка текстуры по типу
            GLuint getType(); // Возвращает тип текстуры
            void setType(GLuint type); // Задает тип текстуры
        protected:
            GLuint handler; // Дескриптор текстуры
            GLuint type; // Тип текстуры, соответствует её слоту
            static std::map<std::string, int> filename_handler; // Получение дескриптора текстуры по её имени
            static std::map<int, int> handler_count; // Получение количества использований по дескриптору текстуры (Shared pointer)
    };

    // Класс 2D текстуры
    class Texture : public BaseTexture
    {
        public:
            Texture(GLuint type = TEX_AVAILABLE_COUNT, const std::string& filename = ""); // Загрузка текстуры с диска или использование "пустой"
            Texture(GLuint width, GLuint height, GLuint attachment, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Конструктор текстуры заданного размера для использования в буфере
            Texture(GLuint width, GLuint height, void* data, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Конструктор текстуры заданного размера без привязки к буферу с загрузкой пикселей по указателю
            Texture(const Texture& other); // Конструктор копирования

            Texture& operator=(const Texture& other); // Оператор присваивания

            void reallocate(GLuint width, GLuint height, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Пересоздает текстуру для имеющегося дескриптора

            virtual void use(); // Привязка текстуры       
    };

    // Класс 3D текстуры
    class TextureArray : public BaseTexture
    {
        public:
            TextureArray(GLuint levels, GLuint width, GLuint height, GLuint attachment, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Конструктор текстуры заданного размера для использования в буфере
            TextureArray(const TextureArray& other); // Конструктор копирования

            TextureArray& operator=(const TextureArray& other); // Оператор присваивания

            void reallocate(GLuint levels, GLuint width, GLuint height, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Пересоздает текстуру для имеющегося дескриптора

            virtual void use(); // Привязка текстуры       
    };

    // Класс кубической текстуры
    class TextureCube : public BaseTexture
    {
        public:
            TextureCube(GLuint type = TEX_AVAILABLE_COUNT, const std::string (&filename)[6] = {""}); // Загрузка текстуры с диска или использование "пустой"
            TextureCube(GLuint width, GLuint height, GLuint attachment, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Конструктор текстуры заданного размера для использования в буфере
            TextureCube(const TextureCube& other); // Конструктор копирования

            TextureCube& operator=(const TextureCube& other); // Оператор присваивания

            void reallocate(GLuint width, GLuint height, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Пересоздает текстуру для имеющегося дескриптора

            virtual void use(); // Привязка текстуры       
    };

    // Класс 3D кубической текстуры
    class TextureCubeArray : public BaseTexture
    {
        public:
            TextureCubeArray(GLuint levels, GLuint width, GLuint height, GLuint attachment, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Конструктор текстуры заданного размера для использования в буфере
            TextureCubeArray(const TextureCubeArray& other); // Конструктор копирования

            TextureCubeArray& operator=(const TextureCubeArray& other); // Оператор присваивания

            void reallocate(GLuint levels, GLuint width, GLuint height, GLuint texType = TEX_DIFFUSE, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLenum dataType = GL_FLOAT); // Пересоздает текстуру для имеющегося дескриптора

            virtual void use(); // Привязка текстуры       
    };
}