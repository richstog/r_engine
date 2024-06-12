#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

namespace r_engine
{    
    std::map<int, int> ShaderProgram::handler_count; // Получение количества использований по дескриптору ШП (Shared pointer)

    ShaderProgram::ShaderProgram()
    {
        program = glCreateProgram();
        handler_count[program] = 1;
    }

    ShaderProgram::ShaderProgram(const ShaderProgram &copy) : program(copy.program)
    {
        handler_count[program]++;
    }

    ShaderProgram::~ShaderProgram()
    {
        if (!--handler_count[program]) // Если количество ссылок = 0
        {
            // Удаление шейдерной программы
            glDeleteProgram(program);
        }
    }

    // Оператор присваивания
    ShaderProgram& ShaderProgram::operator=(const ShaderProgram& other)
    {
        // Если это разные шейдерные программы
        if (program != other.program)
        {
            this->~ShaderProgram(); // Уничтожаем имеющуюся
            // Заменяем новой
            program = other.program;
            handler_count[program]++;
        }
        return *this;
    }

    // Использование шейдеров
    void ShaderProgram::use()
    {
        glUseProgram(program);
    }

    // Функция чтения шейдера из файла
    std::string readFile(const char* filename)
    {
        std::string text;
        std::ifstream file(filename, std::ios::in); // Открываем файл на чтение
        // Если файл доступен и успешно открыт
        if (file.is_open()) 
        { 
            std::stringstream sstr; // Буфер для чтения
            sstr << file.rdbuf(); // Считываем файл
            text = sstr.str(); // Преобразуем буфер к строке
            file.close(); // Закрываем файл
        }

        return text;
    }

    // Функция для загрузки шейдеров
    void ShaderProgram::load(GLuint type, const char* filename)
    {
        // Создание дескрипторов шейдера
        GLuint handler = glCreateShader(type);

        // Переменные под результат компиляции
        GLint result = GL_FALSE;
        int infoLogLength;

        // Считываем текст вершинного шейдера
        std::string code = readFile(filename);
        const char* pointer = code.c_str(); // Преобразование к указателю на const char, так как функция принимает массив си-строк

        // Компиляция кода вершинного шейдера
        glShaderSource(handler, 1, &pointer, NULL);
        glCompileShader(handler);

        // Проверка результата компиляции
        glGetShaderiv(handler, GL_COMPILE_STATUS, &result);
        glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            char* errorMessage = new char[infoLogLength + 1];
            glGetShaderInfoLog(handler, infoLogLength, NULL, errorMessage);
            std::cout << errorMessage;
            delete[] errorMessage;
        }

        // Привязка скомпилированного шейдера
        glAttachShader(program, handler);
        
        // Освобождение дескриптора шейдера
        glDeleteShader(handler);
    }

    // Формирование программы из загруженных шейдеров
    void ShaderProgram::link()
    {
        // Переменные под результат компиляции
        GLint result = GL_FALSE;
        int infoLogLength;

        // Формирование программы из привязанных шейдеров
        glLinkProgram(program);

        // Проверка программы
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            char* errorMessage = new char[infoLogLength + 1];
            glGetProgramInfoLog(program, infoLogLength, NULL, errorMessage);
            std::cout << errorMessage;
            delete[] errorMessage;
        }

        // Используем шейдерную программу объекта из которого вызван метод
        this->use(); 
    }

    // Возвращает местоположение uniform-переменной
    GLuint ShaderProgram::getUniformLoc(const char* name) 
    {
        GLuint result; // Результат
        // Если такую переменную ещё не искали - найдем, иначе вернем уже известный дескриптор
        if (!uniformLocations.count(name))
            uniformLocations[name] = result = glGetUniformLocation(program, name);
        else
            result = uniformLocations[name];

        return result;
    }

    // Привязка uniform-блока
    void ShaderProgram::bindUniformBlock(const char* name, int binding) 
    {
        glUniformBlockBinding( program
                            , glGetUniformBlockIndex(program, name)
                            , binding);
    }

    // Инициализация текстур на шейдере
    void ShaderProgram::bindTextures(const char* textures_base_shader_names[], int count)
    {
        // Цикл по всем доступным текстурам
        for (int i = 0; i < count; i++)
            glUniform1i(getUniformLoc(textures_base_shader_names[i]), i);
    }
}