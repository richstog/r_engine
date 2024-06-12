#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include "Model.hpp"
#include "Camera.hpp"

// Максимальное число источников света
#define MAX_LIGHTS 64
// Стандартное направление источника без поворота
#define DEFAULT_LIGHT_DIRECTION glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)
// Максимальное число образцов для SSAO
#define MAX_SSAO 64

namespace r_engine
{
    // Точечный источник света
    struct LightData 
    {
        alignas(16) glm::vec3 position; // Позиция
        alignas(16) glm::vec3 color; // Цвет 
        alignas(16) glm::vec3 attenuation; // Радиус действия источника, линейный и квадратичный коэф. угасания   
        alignas(16) glm::vec4 direction_angle; // Направление и половинный угол освещенности
        alignas(16) glm::mat4 vp[6]; // Матрицы проекции и трансформации в пространство источника
    };

    // Источник света
    class Light : public Node
    {
        public:
            static int getUBOsize(); // Возвращает размер буфера в байтах
            static void upload(UBO& lights_data); // Загрузка данных в буфер

            static Light& getNew(); // Возвращает ссылку на новый источник света
            void destroy(); // Уничтожает источник света

            static int getCount(); // Возвращает количество источников
            const glm::vec3& c_color() const; // Константный доступ к цвету
            glm::vec3& e_color(); // Неконстантная ссылка для изменений цвета

            const float& c_radius() const; // Константный доступ к радиусу
            float& e_radius(); // Неконстантная ссылка для изменений радиуса

            const float& c_angle() const; // Константный доступ к углу освещенности
            float& e_angle(); // Неконстантная ссылка для изменений угла освещенности

            static void render(ShaderProgram &shaderProgram, UBO &material_buffer); // Рисование отладочных лампочек
        private:
            Light(); // Конструктор без параметров
            Light(const Light& copy) = delete; // Конструктор копирования ОТКЛЮЧЕН
            Light& operator=(const Light& other); // Оператор  присваивания
            virtual ~Light(); 

            glm::vec3 color; // Цвет
            float radius; // Радиус действия источника
            float angle; // Угол полный освещенности 

            int index; // Индекс в массиве отправки (может не совпадать с lights) для дефрагментированного доступа
            static Light& findByIndex(GLuint index); // Возвращает ссылку на источник с нужным индексом

            bool uploadReq; // Необходимость загрузки в следствии изменений
            void check_id(); // Проверка что не взаимодествуем с пустым источником
            void toData(); // Преобразует информацию об источнике в структуру LightData

            virtual void recalcMatrices(); // Метод пересчета матрицы трансформации по необходимости, должен сбрасывать флаг changed
            void recalcVP(); // Пересчитывает по необходимости матрицу вида-проекции

            static GLuint count; // количество используемых источников (должно быть <= MAX_LIGHTS)
            static LightData data[MAX_LIGHTS]; // Массив данных по источникам света
            static Light lights[MAX_LIGHTS]; // Массив источников-узлов сцены
    };

    // Класс направленного источника освещения
    class Sun
    {
        public:
            static Sun& get(); // Доступ к синглтону
            static void upload(UBO& sun_data); // Загрузка данных об источнике в буфер
            
            const glm::vec3& c_direction() const; // Константный доступ к направлению лучей источника
            glm::vec3& e_direction(); // Неконстантная ссылка для изменений направления лучей источника

            const glm::vec3& c_color() const; // Константный доступ к цвету
            glm::vec3& e_color(); // Неконстантная ссылка для изменений цвета

        private:
            Sun(const glm::vec3 &direction = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3 &color = glm::vec3(0.4f, 0.4f, 0.4f));
            
            alignas(16) glm::vec3 direction; // Направление лучей источника
            alignas(16) glm::vec3 color; // Цвет
            alignas(16) glm::mat4 vp[CAMERA_CASCADE_COUNT]; // Матрица вида-проекции источника

            void recalcVP(); // Пересчитывает по необходимости матрицу вида-проекции
            
            static Sun instance; // Экземпляр синглтона
            static bool uploadReq; // Необходимость загрузки в следствии изменений
    };

    // Данные для SSAO
    struct SSAO_data
    {
        float radius = 0.05f;
        float bias = 0.025f;
        int size = MAX_SSAO;
        alignas(16) glm::vec2 scale;
        glm::vec3 samples[MAX_SSAO];
    };
}