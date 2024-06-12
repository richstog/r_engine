#include "VertexArray.hpp"
#include <glad/glad.h>


namespace r_engine
{
    // Создание массива вершин
    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_id);
    }

    // Удаление массива вершин из памяти видеокарты
    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_id);
    }

    // Перемещение массива вершин через оператор равенства с правосторонним значением
    VertexArray& VertexArray::operator=(VertexArray&& vertex_array) noexcept
    {
        m_id = vertex_array.m_id;
        m_elements_count = vertex_array.m_elements_count;
        vertex_array.m_id = 0;
        vertex_array.m_elements_count = 0;
        return *this;
    }

    // Перемещение массива вершин через оператор равенства с правосторонним значением
    VertexArray::VertexArray(VertexArray&& vertex_array) noexcept
    : m_id(vertex_array.m_id), m_elements_count(vertex_array.m_elements_count)
    {
        vertex_array.m_id = 0;
        vertex_array.m_elements_count = 0;
    }

    // Определение работы с данным массивом вершин
    void VertexArray::bind() const
    {
        glBindVertexArray(m_id);
    }

    // Отмена определения работы с данным массивом вершин
    void VertexArray::unbind()
    {
        glBindVertexArray(0);
    }

    // Добавление вершинного буффера в массив вершин
    void VertexArray::add_vertex_buffer(const VertexBuffer& vertex_buffer)
    {
        // Определение работы с данными
        bind();
        vertex_buffer.bind();

        // Цикл, заносящий данные в массив вершин
        for (const BufferElement& current_element : vertex_buffer.get_layout().get_elements())
        {
            glEnableVertexAttribArray(m_elements_count);
            glVertexAttribPointer(
                m_elements_count,
                static_cast<GLint>(current_element.components_count),
                current_element.component_type,
                GL_FALSE,
                static_cast<GLsizei>(vertex_buffer.get_layout().get_stride()),
                reinterpret_cast<const void*>(current_element.offset)
            );
            ++m_elements_count;
        }
    }

    // Добавление индексного буффера в массив вершин
    void VertexArray::set_index_buffer(const IndexBuffer& index_buffer)
    {
        // Определение работы с данными
        bind();
        index_buffer.bind();
        // Получение количества индексов
        m_indexes_count = index_buffer.get_count();
    }
}