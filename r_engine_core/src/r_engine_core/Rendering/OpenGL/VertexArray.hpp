#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

// Определение класса массива вершин
// Описание класса в файле VertexArray.cpp
namespace r_engine
{
    class VertexArray
    {
        public:
        // Конструктор и деструктор
        VertexArray();
        ~VertexArray();

        // Запрет копирования
        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;
        // Реализация move-семантики (Перемешение доступно)
        VertexArray& operator=(VertexArray&& vertex_buffer) noexcept;
        VertexArray(VertexArray&& vertex_buffer) noexcept;

        // Добавление вершинного буффера
        void add_vertex_buffer(const VertexBuffer& vertex_buffer);
        // Добавление индексного буффера
        void set_index_buffer(const IndexBuffer& index_buffer);
        // Функции для работы с массивом вершин
        void bind() const;
        static void unbind();
        // Геттер на количество индексов
        size_t get_indexes_count() const { return m_indexes_count; }

        private:
        // Номер массива вершин, выданный OpenGL
        unsigned int m_id = 0;
        // Количество вершин
        unsigned int m_elements_count = 0;
        // Количество индексов
        size_t m_indexes_count = 0;
    };
}
