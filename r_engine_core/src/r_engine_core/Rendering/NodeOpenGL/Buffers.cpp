#include "Buffers.hpp"

namespace r_engine
{
    // Счетчики использований дескрипторов
    std::map<GLuint, GLuint> VAO::handler_count; 
    std::map<GLuint, GLuint> BO::handler_count; 

    // Создает VAO и активирует его
    VAO::VAO()
    { 
        glGenVertexArrays(1, &handler); // Генерация одного объекта массива вершин
        glBindVertexArray(handler);     // Привязка для использования
        handler_count[handler] = 1;     // Инициализация счетчика для дескриптора
    }

    // Уничтожает VAO
    VAO::~VAO()
    {
        // Если дескриптор никем не используется - освободим его
        if (!--handler_count[handler])
        {
            glDeleteVertexArrays(1, &handler);
            handler_count.erase(handler); // Удаление из словаря
        }
    }

    // Конструктор копирования
    VAO::VAO(const VAO & copy) : handler(copy.handler)
    {
        handler_count[handler]++;
    }

    // Оператор присваивания
    VAO& VAO::operator=(const VAO & other) 
    {
        // Если это разные дескрипторы
        if (handler != other.handler)
        { // то следуюет удалить текущий перед заменой
            this->~VAO(); 
            handler = other.handler;
            handler_count[handler]++;
        }

        return *this;
    }

    // Активация VAO
    void VAO::use()
    {
        glBindVertexArray(handler); // Привязка VAO для использования
    }

    // Деактивация активного VAO
    void VAO::disable()
    {
        glBindVertexArray(0);       // Отключение VAO
    }

    // Создает пустой буфер заданного типа
    BO::BO(BUFFER_TYPE t) : type(t)
    {
        glGenBuffers(1, &handler); // Генерация одного объекта буфера
        handler_count[handler] = 1;
        use(); // Привязка буфера
    }

    // Создает и загружает туда данные
    BO::BO(BUFFER_TYPE t, const void *data, int size) : BO(t)
    {
        load(data, size);
    }

    // Уничтожает буфер
    BO::~BO()
    {
        if (handler) // Если буфер был создан
        {
            // Если дескриптор никем не используется - освободим его
            if (!--handler_count[handler])
            {
                glDeleteBuffers(1, &handler);
                handler_count.erase(handler); // Удаление из словаря
            }
            handler = 0;
        }
    }

    // Конструктор копирования
    BO::BO(const BO & copy) : handler(copy.handler), type(copy.type)
    {
        handler_count[handler]++;
    }

    // Оператор присваивания
    BO& BO::operator=(const BO & other)
    {
        // Если это разные дескрипторы
        if (handler != other.handler)
        { // то следуюет удалить текущий перед заменой
            this->~BO(); 
            handler = other.handler;
            handler_count[handler]++;
        }
        // Изменим тип
        type = other.type;

        return *this;
    }

    // Загрузка вершин в буфер
    void BO::load(const void *data, int size, GLuint mode)
    {
        use(); // Привязка буфера
        glBufferData(type, size, data, mode);
    }

    void BO::use()
    {
        glBindBuffer(type, handler); // Привязка элементного буфера
    }

    // Создает пустой uniform-буфер заданного размера с автоматической привязкой
    UBO::UBO(int size, int binding) : BO(UNIFORM, 0, size)
    {
        rebind(binding);
    }

    // Создает пустой uniform-буфер заданного размера с автоматической привязкой
    UBO::UBO(const void *data, int size, int binding) : BO(UNIFORM, data, size)
    {
        rebind(binding);
    }

    // перепривязка
    void UBO::rebind(int binding)
    {
        glBindBufferBase(type, binding, handler); 
    }

    // Загрузка с отступом
    void UBO::loadSub(const void *data, int size, int offset)
    {
        use();
        glBufferSubData(type, offset, size, data);
    }

    // Создает буфер кадра с нужным числом прикреплений текстур
    FBO::FBO(GLuint *attachments, int count)
    {
        glGenFramebuffers(1, &handler);
        use();
        glDrawBuffers(count, attachments);
    }

    // Уничтожение буфера
    FBO::~FBO()
    {
        glDeleteFramebuffers(1, &handler);
    }

    // Активирует буфер кадра в заданном режиме
    void FBO::use(GLuint mode) 
    {
        glBindFramebuffer(mode, handler);
    }

    // Активирует базовый буфер в заданном режиме
    void FBO::useDefault(GLuint mode)
    {
        glBindFramebuffer(mode, 0);
    } 

    // Привязка рендер буфера
    void FBO::assignRenderBuffer(GLuint hander, GLuint attachment)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, hander);
    }

    // Создает буфер рендера с заданными параметрами размеров и используемых компонент
    RBO::RBO(int w, int h, GLuint component)
    {
        glGenRenderbuffers(1, &handler);
        glBindRenderbuffer(GL_RENDERBUFFER, handler);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    }

    // Уничтожение буфера
    RBO::~RBO()
    {
        glDeleteRenderbuffers(1, &handler);
    }

    // Изменяет размеры буфера рендера
    void RBO::reallocate(int w, int h, GLuint component)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, handler); // Привязка элементного буфера
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    }

    // Возвращает дескриптор буфера рендера
    GLuint RBO::getHandler()
    {
        return handler;
    }
}