#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace r_engine
{
    std::map<std::string, int> BaseTexture::filename_handler; // Получение дескриптора текстуры по её имени
    std::map<int, int> BaseTexture::handler_count; // Получение количества использований по дескриптору текстуры (Shared pointer)

    // Загрузка текстуры с диска или использование "пустой"
    Texture::Texture(GLuint t, const std::string& filename)
    {
        type = t;
        if (!filename_handler.count(filename))
        {
            std::string empty = "";
            int width, height, channels; // Ширина, высота и цветовые каналы текстуры
            unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, STBI_default); // Загрузка в оперативную память изображения
            // Если изображение успешно счиитано с диска или отсутствует пустая текстура
            if (image || !filename_handler.count(empty))
            {
                glActiveTexture(type + GL_TEXTURE0);
                glGenTextures(1, &handler); // Генерация одной текстуры
                glBindTexture(GL_TEXTURE_2D, handler); // Привязка текстуры как активной

                filename_handler[filename] = handler; // Запоминим её дескриптор для этого имени файла
                handler_count[handler] = 0; // Создадим счетчик использований дескриптора, который будет изменен в конце

                // Если изображение успешно считано
                if (image)
                {
                    // Загрузка данных с учетом прозрачности
                    if (channels == 3) // RGB
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); 
                    else if (channels == 4) // RGBA
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

                    glGenerateMipmap(GL_TEXTURE_2D); // Генерация мипмапа для активной текстуры
                    glBindTexture(GL_TEXTURE_2D, 0); // Отвязка активной текстуры
                    
                    stbi_image_free(image); // Освобождение оперативной памяти
                }
                // Иначе изображение не считано и надо создать пустую текстуру
                else
                {
                    image = new unsigned char[3] {255,255,255}; // RGB по 1 байту на 
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, image); // Загрузка данных на видеокарту
                    delete[] image; // Освобождение оперативной памяти
                    
                    filename_handler[empty] = handler; // Запоминим дополнительно её дескриптор для NULL-строки
                }
            }
            // Иначе используем существующую пустую текстуру (текстура не загружена, пустую создавать не нужно)
            else
                handler = filename_handler[empty];
        }
        // Иначе используем уже существующую по имени файла
        else
            handler = filename_handler[filename];
            
        handler_count[handler]++;
    }

    // Конструктор текстуры заданного размера для использования в буфере
    Texture::Texture(GLuint width, GLuint height, GLuint attachment, GLuint texType, GLint internalformat, GLint format, GLenum dataType) 
    {
        type = texType;
        // Генерация текстуры заданного размера
        glGenTextures(1, &handler);
        glBindTexture(GL_TEXTURE_2D, handler);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, dataType, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Привязка к буферу кадра
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handler, 0);

        // Создаем счетчик использований дескриптора
        handler_count[handler] = 1;
    }

    // Конструктор текстуры заданного размера без привязки к буферу с загрузкой пикселей по указателю
    Texture::Texture(GLuint width, GLuint height, void* data, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        type = texType;
        // Генерация текстуры заданного размера
        glGenTextures(1, &handler);
        glBindTexture(GL_TEXTURE_2D, handler);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, dataType, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Создаем счетчик использований дескриптора
        handler_count[handler] = 1;
    }

    // Конструктор копирования
    Texture::Texture(const Texture& other)
    {
        handler = other.handler;
        type = other.type;
        // Делаем копию и увеличиваем счетчик
        handler_count[handler]++;
    }

    // Оператор присваивания
    Texture& Texture::operator=(const Texture& other)
    {
        // Если это разные текстуры
        if (handler != other.handler)
        {
            this->~Texture(); // Уничтожаем имеющуюся
            // Заменяем новой
            handler = other.handler;
            handler_count[handler]++;
        }
        type = other.type;

        return *this;
    }

    BaseTexture::~BaseTexture()
    {
        if (!--handler_count[handler]) // Если количество ссылок = 0
        {
            glDeleteTextures(1, &handler); // Удаление текстуры
            // Удаление из словаря имен файлов и дескрипторов
            for (auto it = filename_handler.begin(); it != filename_handler.end();)
            {
                if (it->second == handler) 
                    it = filename_handler.erase(it);
                else
                    it++;
            }
        }
    }

    // Пересоздает текстуру для имеющегося дескриптора
    void Texture::reallocate(GLuint width, GLuint height, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        use();
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, dataType, NULL);
    }

    // Привязка текстуры
    void Texture::use()
    {
        glActiveTexture(type + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, handler); // Привязка текстуры как активной
    }

    // Отвязка текстуры по типу
    void BaseTexture::disable(GLuint type)
    {
        glActiveTexture(type + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0); // Отвязка текстуры
    }

    // Возвращает тип текстуры
    GLuint BaseTexture::getType()
    {
        return type;
    }

    // Задает тип текстуры
    void BaseTexture::setType(GLuint type)
    {
        this->type = type;
    }

    // Конструктор текстуры заданного размера для использования в буфере
    TextureArray::TextureArray(GLuint levels, GLuint width, GLuint height, GLuint attachment, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        type = texType;
        // Генерация текстуры заданного размера
        glGenTextures(1, &handler);
        glBindTexture(GL_TEXTURE_2D_ARRAY, handler);
        glTexImage3D(
            GL_TEXTURE_2D_ARRAY, 0, internalformat, width, height, levels, 0, format, dataType, 0);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Привязка к буферу кадра
        glFramebufferTexture(GL_FRAMEBUFFER, attachment, handler, 0);

        // Создаем счетчик использований дескриптора
        handler_count[handler] = 1;
    }

    // Конструктор копирования
    TextureArray::TextureArray(const TextureArray& other)
    {
        handler = other.handler; 
        type = other.type;
        // Делаем копию и увеличиваем счетчик
        handler_count[handler]++;
    }

    // Оператор присваивания
    TextureArray& TextureArray::operator=(const TextureArray& other)
    {
        // Если это разные текстуры
        if (handler != other.handler)
        {
            this->~TextureArray(); // Уничтожаем имеющуюся
            // Заменяем новой
            handler = other.handler;
            handler_count[handler]++;
        }
        type = other.type;

        return *this;
    }

    // Пересоздает текстуру для имеющегося дескриптора
    void TextureArray::reallocate(GLuint levels, GLuint width, GLuint height, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        use();
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, dataType, NULL);
    }

    // Привязка текстуры
    void TextureArray::use()
    {
        glActiveTexture(type + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, handler); // Привязка текстуры как активной
    }

    // Загрузка текстуры с диска или использование "пустой"
    TextureCube::TextureCube(GLuint t, const std::string (&filename)[6])
    {
        type = t;
        std::string complex_name;
        for (int i = 0; i < 6; i++)
            complex_name += filename[i];
        if (!filename_handler.count(complex_name))
        {
            std::string empty = "";
            int width, height, channels; // Ширина, высота и цветовые каналы текстуры
            unsigned char* image;

            glActiveTexture(type + GL_TEXTURE0);
            glGenTextures(1, &handler); // Генерация одной текстуры
            glBindTexture(GL_TEXTURE_CUBE_MAP, handler); // Привязка текстуры как активной

            filename_handler[complex_name] = handler; // Запомним её дескриптор для этого имени файла
            handler_count[handler] = 0; // Создадим счетчик использований дескриптора, который будет изменен в конце

            for (int i = 0; i < 6; i++) 
            {
                image = stbi_load(filename[i].c_str(), &width, &height, &channels, STBI_default); // Загрузка в оперативную память изображения

                // Если изображение успешно считано
                if (image)
                {
                    // Загрузка данных с учетом прозрачности
                    if (channels == 3) // RGB
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                    else if (channels == 4) // RGBA
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

                    stbi_image_free(image); // Освобождение оперативной памяти
                }
                // Иначе изображение не считано и надо создать пустую текстуру
                else
                {
                    image = new unsigned char[3] {255,255,255}; // RGB по 1 байту на 
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, image); // Загрузка данных на видеокарту
                    delete[] image; // Освобождение оперативной памяти
                }
            }
        }
        // Иначе используем уже существующую по имени файла
        else
            handler = filename_handler[complex_name];

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        handler_count[handler]++;
    }

    // Конструктор текстуры заданного размера для использования в буфере
    TextureCube::TextureCube(GLuint width, GLuint height, GLuint attachment, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        type = texType;
        // Генерация текстуры заданного размера
        glGenTextures(1, &handler);
        glBindTexture(GL_TEXTURE_CUBE_MAP, handler);
        for (int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, dataType, 0);  

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Привязка к буферу кадра
        glFramebufferTexture(GL_FRAMEBUFFER, attachment, handler, 0);

        // Создаем счетчик использований дескриптора
        handler_count[handler] = 1;
    }

    // Конструктор копирования
    TextureCube::TextureCube(const TextureCube& other)
    {
        handler = other.handler; 
        type = other.type;
        // Делаем копию и увеличиваем счетчик
        handler_count[handler]++;
    }

    // Оператор присваивания
    TextureCube& TextureCube::operator=(const TextureCube& other)
    {
        // Если это разные текстуры
        if (handler != other.handler)
        {
            this->~TextureCube(); // Уничтожаем имеющуюся
            // Заменяем новой
            handler = other.handler;
            handler_count[handler]++;
        }
        type = other.type;

        return *this;
    }

    // Пересоздает текстуру для имеющегося дескриптора
    void TextureCube::reallocate(GLuint width, GLuint height, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        use();
        for (int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, dataType, 0);  
    }

    // Привязка текстуры
    void TextureCube::use()
    {
        glActiveTexture(type + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, handler); // Привязка текстуры как активной
    }

    // Конструктор текстуры заданного размера для использования в буфере
    TextureCubeArray::TextureCubeArray(GLuint levels, GLuint width, GLuint height, GLuint attachment, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        type = texType;
        // Генерация текстуры заданного размера
        glGenTextures(1, &handler);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, handler);
        glTexImage3D(
            GL_TEXTURE_CUBE_MAP_ARRAY, 0, internalformat, width, height, 6*levels, 0, format, dataType, 0);

        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Привязка к буферу кадра
        glFramebufferTexture(GL_FRAMEBUFFER, attachment, handler, 0);

        // Создаем счетчик использований дескриптора
        handler_count[handler] = 1;
    }

    // Конструктор копирования
    TextureCubeArray::TextureCubeArray(const TextureCubeArray& other)
    {
        handler = other.handler; 
        type = other.type;
        // Делаем копию и увеличиваем счетчик
        handler_count[handler]++;
    }

    // Оператор присваивания
    TextureCubeArray& TextureCubeArray::operator=(const TextureCubeArray& other)
    {
        // Если это разные текстуры
        if (handler != other.handler)
        {
            this->~TextureCubeArray(); // Уничтожаем имеющуюся
            // Заменяем новой
            handler = other.handler;
            handler_count[handler]++;
        }
        type = other.type;

        return *this;
    }

    // Пересоздает текстуру для имеющегося дескриптора
    void TextureCubeArray::reallocate(GLuint levels, GLuint width, GLuint height, GLuint texType, GLint internalformat, GLint format, GLenum dataType)
    {
        use();
        glTexImage3D(
            GL_TEXTURE_CUBE_MAP_ARRAY, 0, internalformat, width, height, 6*levels, 0, format, dataType, 0);
    }

    // Привязка текстуры
    void TextureCubeArray::use()
    {
        glActiveTexture(type + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, handler); // Привязка текстуры как активной
    }
}