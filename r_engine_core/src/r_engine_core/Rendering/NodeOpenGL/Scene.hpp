#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <list>

#include "Model.hpp"
#include "Camera.hpp"

#define DEFAULT_MTL_DIR "./"

namespace r_engine
{
    class Scene loadOBJtoScene(const char* filename, const char* mtl_directory = DEFAULT_MTL_DIR, const char* texture_directory = DEFAULT_MTL_DIR);

    // Класс сцены
    class Scene
    {
        public:
            Scene(); // Конструктор пустой сцены
            Scene(const Scene &copy); // Конструктор копирования
            Scene& operator=(const Scene& other); // Оператор присваивания

            void render(ShaderProgram &shaderProgram, UBO &material_buffer); // Рендер сцены

            Node root; // Корневой узел

            // Списки объектов, выступающих узлами
            std::list<Node> nodes; // Список пустых узлов
            std::list<Model> models; // Список моделей для рендера
            std::list<Camera> cameras; // Список камер
        
        protected:
            void rebuld_tree(const Scene& from); // Перестройка дерева после копирования или присваивания
            template <class T>
            void rebuild_Nodes_list(T& nodes, const Scene& from); // Перестройка узлов выбранного списка
            template <class T>
            void move_parent(Node& for_node, const std::list<T>& from_nodes, std::list<T>& this_nodes); // Сдвигает родителя узла между двумя списками при условии его принадлежности к оригинальному
    };
}