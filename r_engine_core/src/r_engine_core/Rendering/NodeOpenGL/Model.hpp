#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include "Buffers.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace r_engine
{
    class Model genShpere(float radius, int sectorsCount, class Node* parent = NULL); // Генерирует сферу заданного радиуса с определенным количеством сегментов

    // Класс узла сцены
    class Node 
    {
        public:
            Node(Node* parent = NULL); // Конструктор с заданным родителем (по умолчанию NULL)
            Node(const Node& copy); // Конструктор копирования
            Node& operator=(const Node& other); // Оператор присваивания
            virtual ~Node();

            void setParent(Node * parent); // Устанавливает родителя для узла

            virtual const glm::mat4& getTransformMatrix(); // Возвращает матрицу трансформации модели
            bool isChanged(); // Возвращает необходимость пересчета матрицы трансформации

            const glm::vec3& c_position() const; // Константный доступ к позиции
            const glm::quat& c_rotation() const; // Константный доступ к повороту
            const glm::vec3& c_scale() const; // Константный доступ к масштабированию
            virtual glm::vec3& e_position(); // Неконстантная ссылка для изменений позиции
            virtual glm::quat& e_rotation(); // Неконстантная ссылка для изменений поворота
            virtual glm::vec3& e_scale(); // Неконстантная ссылка для изменений масштабирования 

            Node* getParent(); // Возвращает указатель на родителя
            const std::vector<Node*>& getChildren() const; // Возвращает ссылку на вектор дочерних узлов

        protected:
            Node *parent; // Родительский узел
            std::vector<Node*> children; // Узлы-потомки !Не должны указывать на NULL!

            glm::vec3 position; // позиция модели
            glm::quat rotation; // поворот модели
            glm::vec3 scale;    // масштабирование модели

            bool changed; // Флаг необходимости пересчета матрицы трансформации
            glm::mat4 transform; // Матрица трансформации модели
            bool parent_changed; // Флаг изменений у родителя - необходимость пересчета итоговой трансформации
            glm::mat4 result_transform; // Итоговая трансформация с учетом родительской

            virtual void recalcMatrices(); // Метод пересчета матрицы трансформации по необходимости, должен сбрасывать флаг changed
            void invalidateParent(); // Проход потомков в глубину с изменением флага parent_changed
    };

    // Материал модели
    struct Material
    {
        alignas(16) glm::vec3 ka; // коэф. фонового отражения (цвет фонового освещения)
        alignas(16) glm::vec3 kd; // коэф. диффузного отражения (цвет объекта)
        alignas(16) glm::vec3 ks; // коэф. зеркального блика
        float p; // показатель глянцевости
        // Значения по умолчанию
        Material() : ka(0.2f), kd(0.2f), ks(0.2f), p(1) { };
    };

    // Класс модели
    class Model : public Node
    {
        public:
            Model(Node *parent = NULL); // Конструктор по умолчанию
            Model(const Model& copy); // Конструктор копирования
            Model& operator=(const Model& other); // Оператор присваивания
            virtual ~Model();

            void render(); // Вызов отрисовки без uniform-данных
            void render(ShaderProgram &shaderProgram, UBO &material_buffer); // Вызов отрисовки

            void load_verteces(glm::vec3* verteces, GLuint count); // Загрузка вершин в буфер
            void load_indices(GLuint* indices, GLuint count); // Загрузка индексов в буфер
            void load_texCoords(glm::vec2* texCoords, GLuint count); // Загрузка текстурных координат в буфер
            void load_normals(glm::vec3* normals, GLuint count); // Загрузка нормалей в буфер
            void set_index_range(size_t first_byteOffset, size_t count); // Ограничение диапазона из буфера индексов
            void set_texture(Texture& texture); // Привязка текстуры к модели
            
            Material material; // Материал модели
        private:
            VAO vao;
            BO vertex_vbo, index_vbo; // вершинный и индексный буферы
            BO normals_vbo, texCoords_vbo; // буферы с нормалями и текстурными координатами
            GLuint verteces_count; // Количество вершин
            size_t first_index_byteOffset, indices_count; // Сдвиг в байтах для первого и количество индексов
            Texture texture_diffuse; // Диффузная текстура
            Texture texture_ambient; // Текстура фонового освщения
            Texture texture_specular; // Текстура зеркального отражения
    };
}