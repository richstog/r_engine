#pragma once

// Определение класса двухмерной текстуры
// Описание класса в файле Texture2D.cpp
namespace r_engine
{
    class Texture2D
    {
        public:
        // Конструктор с параметрами и деструктор
        Texture2D(const unsigned char* data, const unsigned int width, const unsigned int height);
        ~Texture2D();

        // Запрет копирования текстуры
        Texture2D(const Texture2D&) = delete;
        Texture2D& operator= (const Texture2D&) = delete;

        // Реализация move-семантики (Перемешение доступно)
        Texture2D& operator= (Texture2D&& texture) noexcept;
        Texture2D(Texture2D&& texture) noexcept;

        // Функции для работы с текстурой
        void bind(const unsigned unit) const;

        private:
        // Номер текстуры, выданный OpenGL
        unsigned int m_id = 0;
        // Ширина текстуры
        unsigned int m_width = 0;
        // Высота текстуры
        unsigned int m_height = 0;
    };
}