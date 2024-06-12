#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <map>

namespace r_engine
{
    // Объект массива вершин
    class VAO
    {
        public:
            VAO(); // Создает VAO и активирует его
            ~VAO(); // Уничтожает VAO
            VAO(const VAO & copy); // Конструктор копирования
            VAO& operator=(const VAO & other); // Оператор присваивания

            void use(); // Активация VAO
            static void disable(); // Деактивация активного VAO

        private:
            GLuint handler; // Дескриптор
            static std::map<GLuint, GLuint> handler_count; // Счетчик использований дескриптора
    };

    // Тип буфера
    enum BUFFER_TYPE {
        VERTEX = GL_ARRAY_BUFFER,
        ELEMENT = GL_ELEMENT_ARRAY_BUFFER,
        UNIFORM = GL_UNIFORM_BUFFER
    };

    // Объект вершинного буфера
    class BO
    {
        public:
            BO(BUFFER_TYPE type); // Создает пустой буфер заданного типа
            BO(BUFFER_TYPE type, const void *data, int size); // Создает и загружает туда данные
            ~BO(); // Уничтожает буфер
            BO(const BO & copy); // Конструктор копирования
            BO& operator=(const BO & other); // Оператор присваивания

            void load(const void *data, int size, GLuint mode = GL_STATIC_DRAW); // Загрузка данных в буфер
            void use(); 

        protected:
            GLuint handler; // Дескриптор
            BUFFER_TYPE type; // Тип буфера
        private:
            static std::map<GLuint, GLuint> handler_count; // Счетчик использований дескриптора
    };

    // Объект uniform-буфера
    class UBO : public BO
    {
        public:
            UBO(int size, int binding); // Создает пустой uniform-буфер заданного размера с автоматической привязкой
            UBO(const void *data, int size, int binding); // Создает пустой uniform-буфер заданного размера с автоматической привязкой

            void rebind(int binding); // Перепривязка
            void loadSub(const void *data, int size, int offset = 0); // Загрузка с отступом
    };

    // Объект буфера кадра
    class FBO
    {
        public:
            FBO(GLuint *attachments = 0, int count = 0); // Создает буфер кадра с нужным числом прикреплений текстур
            ~FBO(); // Уничтожение буфера

            void use(GLuint mode = GL_FRAMEBUFFER); // Активирует буфер кадра в заданном режиме
            static void useDefault(GLuint mode = GL_FRAMEBUFFER); // Активирует базовый буфер в заданном режиме
            void assignRenderBuffer(GLuint hander, GLuint attachment = GL_DEPTH_ATTACHMENT); // Привязка рендер буфера
        protected:
            GLuint handler; // Дескриптор
    };

    // Объект буфера рендера
    class RBO
    {
        public:
            RBO(int w, int h, GLuint component = GL_DEPTH_COMPONENT); // Создает буфер рендера с заданными параметрами размеров и используемых компонент
            ~RBO(); // Уничтожение буфера

            void reallocate(int w, int h, GLuint component = GL_DEPTH_COMPONENT); // Изменяет размеры буфера рендера

            GLuint getHandler(); // Возвращает дескриптор буфера рендера
        protected:
            GLuint handler; // Дескриптор
    };
}