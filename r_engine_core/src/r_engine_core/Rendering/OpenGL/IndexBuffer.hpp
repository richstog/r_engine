#pragma once

#include "VertexBuffer.hpp"

// Определение класса индексного буффера
// Описание класса в файле IndexBuffer.cpp
namespace r_engine
{
    class IndexBuffer
    {
        public:
        // Определение конструкторов и деструкторов
        IndexBuffer();
        IndexBuffer(const void* data, const size_t count, const VertexBuffer::EUsage usage = VertexBuffer::EUsage::Static);
        ~IndexBuffer();

        // Реализация move-семантики
        IndexBuffer& operator=(IndexBuffer&& index_buffer) noexcept;
        IndexBuffer(IndexBuffer&& index_buffer) noexcept;

        // Функции для работы с индексным буффером
        void bind() const;
        static void unbind();
        size_t get_count() const { return m_count; }

        private:
        // Номер буффера, выданный OpenGL
        unsigned int m_id = 0;
        // Количество индексов
        size_t m_count;
    };
}