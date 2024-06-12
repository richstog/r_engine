#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Model.hpp"
    
// Ближняя граница области отсечения
#define CAMERA_NEAR 0.1f
// Дальняя граница области отсечения
#define CAMERA_FAR 100.0f
// Вектор, задающий верх для камеры
#define CAMERA_UP_VECTOR glm::vec3(0.0f, 1.0f, 0.0f)
// Вектор, задающий стандартный поворот углами Эйлера (в положительном направлении оси Z)
#define CAMERA_DEFAULT_ROTATION glm::vec3(0.0f, 180.0f, 0.0f)
// Стандартный угол обзора 
#define CAMERA_FOVy 60.0f
// Стандартная чувствительность
#define CAMERA_DEFAULT_SENSIVITY 0.005f
// Количество каскадов для карт теней
#define CAMERA_CASCADE_COUNT 4

// Данные о дистанциях каскадов 
extern const float camera_cascade_distances[];

namespace r_engine
{
    // Данные о камере для шейдера
    struct CameraData
    {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 position;
    };

    // Класс камеры
    class Camera : public Node
    {
        public:
            Camera(float aspect, const glm::vec3 &position = glm::vec3(0.0f), const glm::vec3 &initialRotation = CAMERA_DEFAULT_ROTATION, float fovy = CAMERA_FOVy); // Конструктор камеры с проекцией перспективы
            Camera(float width, float height, const glm::vec3 &position = glm::vec3(0.0f), const glm::vec3 &initialRotation = CAMERA_DEFAULT_ROTATION); // Конструктор ортографической камеры
            Camera(const Camera& copy); // Конструктор копирования камеры
            Camera& operator=(const Camera& other); // Оператор присваивания
            virtual ~Camera(); // Деструктор

            const glm::mat4& getVP(); // Возвращает ссылку на константную матрицу произведения матриц вида и проекции
            const glm::mat4& getProjection(); // Возвращает ссылку на константную матрицу проекции 
            const glm::mat4& getView(); // Возвращает ссылку на константную матрицу вида
            
            void rotate(const glm::vec2 &xyOffset); // Поворачивает камеру на dx и dy пикселей с учетом чувствительности
            
            void setPerspective(float fov, float aspect); // Устанавливает заданную матрицу перспективы
            void setOrtho(float width, float height); // Устанавливает заданную ортографическую матрицу
            void setSensitivity(float sensitivity); // Изменяет чувствительность мыши
            const float& getSensitivity() const; // Возвращает чувствительность мыши
            
            void use(); // Использование этой камеры как текущей
            static Camera& current(); // Ссылка на текущую используемую камеру
            
            CameraData& getData(); // Данные о камере для шейдера
            std::pair<bool, const glm::vec4(*)[8]> getProjCoords(); // Доступ к координатам с флагом изменения, описывающим пространство вида с пересчетом, если это требуется
        protected:
            Camera(const glm::vec3 &position, const glm::vec3 &initialRotation); // Защищенный (protected) конструктор камеры без перспективы 

            glm::mat4 view; // Матрица вида
            glm::mat4 projection; // Матрица проекции
            glm::mat4 vp; // Матрица произведения вида и проекции
            bool requiredRecalcVP; // Необходимость пересчета матрицы вида и проекции камеры
            bool requiredRecalcCoords; // Необходимость пересчета точек, описывающих пространство камеры
            glm::vec4 coords[CAMERA_CASCADE_COUNT][8]; // Координаты, описывающие пространство камеры
            glm::mat4 cascade_proj[CAMERA_CASCADE_COUNT]; // Матрицы проекций каскадов

            float sensitivity; // Чувствительность мыши
            
            virtual void recalcMatrices(); // Метод пересчета матрицы вида и произведения Вида*Проекции по необходимости, должен сбрасывать флаг changed

            static Camera* p_current; // Указатель на текущую используемую камеру
    };
}