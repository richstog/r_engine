#pragma once

#include <initializer_list>

#include <vector>

#include <stdlib.h>
#include <glad/glad.h>

// Определение класса и структур для работы с вершинным буфером
// Описание класса в файле VertexBuffer.cpp
namespace r_engine
{
    // Перечисление типов данных, которыми могут являться элементы вершинного буфера
    enum class ShaderDataType
    {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4
    };

    // Структура элемента буффера
    struct BufferElement
    {
        ShaderDataType type;
        uint32_t component_type;
        size_t components_count;
        size_t size;
        size_t offset;

        BufferElement(const ShaderDataType type);
    };

    // Класс "оболочки" буфера
    // В зависимости от типов данных и их порядка, выполняется создание буфера вершин
    // Пример работы с BufferLayout описан в файле Application.cpp на 435 строчке
    class BufferLayout
    {
        public:
        BufferLayout(std::initializer_list<BufferElement> elements)
        : m_elements(std::move(elements))
        {
            size_t offset = 0;
            m_stride = 0;
            for (auto& element : m_elements)
            {
                element.offset = offset;
                offset += element.size;
                m_stride += element.size;
            }
        }

        const std::vector<BufferElement>& get_elements() const { return m_elements; }
        size_t get_stride() const { return m_stride; }

        private:
        std::vector<BufferElement> m_elements;
        size_t m_stride = 0;
    };

    // Класс вершинного буфера
    class VertexBuffer
    {
        public:
        // Типы того, как будет использоваться буффер
        enum class EUsage
        {
            Static,
            Dynamic,
            Stream,
        };

        // Конструктор с параметрами и деконструктор
        VertexBuffer(const void* data, const size_t size, BufferLayout buffer_layout,const EUsage usage = VertexBuffer::EUsage::Static);
        ~VertexBuffer();
        // Конструктор без параметров
        VertexBuffer();

        // Реализация move-семантики (Перемешение доступно)
        VertexBuffer& operator=(VertexBuffer&& vertex_buffer) noexcept;
        VertexBuffer(VertexBuffer&& vertex_buffer) noexcept;

        // Функции для работы с вершинным буфером
        void bind() const;
        static void unbind();

        // Геттер на "оболочку" буфера
        const BufferLayout& get_layout() const { return m_buffer_layout; }

        private:
        // Номер буффера вершин, выданный OpenGL
        GLuint m_id = 0;
        // "Оболочка" буффера
        BufferLayout m_buffer_layout;
    };
}