#include "Scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

        
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE 
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tinygltf/tiny_gltf.h>

#include <functional>

#include <glm/gtc/type_ptr.hpp>

namespace r_engine
{    
    // Конструктор пустой сцены
    Scene::Scene()
    {

    }

    // Конструктор копирования
    Scene::Scene(const Scene &copy): root(copy.root), 
    nodes(copy.nodes), models(copy.models), cameras(copy.cameras)
    {
        rebuld_tree(copy);
    } 

    // Оператор присваивания
    Scene& Scene::operator=(const Scene& other) 
    {
        root = other.root;
        nodes = other.nodes;
        models = other.models;
        cameras = other.cameras;

        rebuld_tree(other);

        return *this;
    } 

    // Рендер сцены
    void Scene::render(ShaderProgram &shaderProgram, UBO &material_buffer) 
    {
        for (auto & model : models)
            model.render(shaderProgram, material_buffer);
    }

    // Перестройка узлов выбранного списка
    template <class T>
    void Scene::rebuild_Nodes_list(T& nodes, const Scene& from)
    {
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            // Берем родителя, который указывает на оригинальный объект
            Node* parent = it->getParent();
            
            // Если родитель - оригинальный корневой узел, то меняем на собственный корневой узел
            if (parent == &from.root) 
            {
                it->setParent(&root);
                continue;
            }

            // Если можно привести к модели, то ищем родителя среди моделей
            if (dynamic_cast<Model*>(parent))
                move_parent(*it, from.models, this->models);
            else
            // Иначе проверяем на принадлежность к камерам
            if (dynamic_cast<Camera*>(parent))
                move_parent(*it, from.cameras, this->cameras);
            // Иначе это пустой узел
            else
                move_parent(*it, from.nodes, this->nodes);
                
            // Не нашли родителя - значит он не часть этой сцены 
            // и изменений по нему не требуется
        }
    } 

    // Сдвигает родителя узла между двумя списками при условии его принадлежности к оригинальному
    template <class T>
    void Scene::move_parent(Node& for_node, const std::list<T>& from_nodes, std::list<T>& this_nodes)
    {
        // Возьмем адрес родителя
        Node* parent = for_node.getParent();
        // Цикл по элементам списков для перемещения родителя
        // Списки в процессе копирования идеинтичные, вторая проверка не требуется
        for (auto it_from = from_nodes.begin(), it_this = this_nodes.begin(); it_from != from_nodes.end(); ++it_from, ++it_this)
            // Если адрес объекта, на который указывает итератор, совпадает с родителем - меняем родителя по второму итератору (it_this)
            if (&(*it_from) == parent)
                for_node.setParent(&(*it_this));
    }

    // Перестройка дерева после копирования или присваивания
    void Scene::rebuld_tree(const Scene& from)
    {    
        // Восстановим родителей в пустых узлах для копии
        rebuild_Nodes_list(nodes, from);
        rebuild_Nodes_list(models, from);
        rebuild_Nodes_list(cameras, from);
    }

    inline void hash_combine(std::size_t& seed) { }

    template <typename T, typename... Rest>
    inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        hash_combine(seed, rest...);
    }

    Scene loadOBJtoScene(const char* filename, const char* mtl_directory, const char* texture_directory)
    {
        Scene result;
        Model model;
        // Все модели образованные на основании этой модели будут иметь общего родителя
        model.setParent(&result.root); 

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string err;
        std::string warn;

        // Значение гамма-коррекции
        extern float inv_gamma;

        // Если в процессе загрузки возникли ошибки - выдадим исключение
	    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, mtl_directory))
	        throw std::runtime_error(err);
    
        std::vector<GLuint>    indices;  // индексы модели
        std::vector<glm::vec3> verteces; // вершины
        std::vector<glm::vec3> normals; // нормали
        std::vector<glm::vec2> texCords; // текстурные координаты
        size_t hash; // Для уникальных вершин
        std::map <int, int> uniqueVerteces; // словарь для уникальных вершин: ключ - хеш, значение - индекс вершины
        
        int last_material_index = 0; // индекс последнего материала (для группировки моделей)
        int count = 0, offset; // для индексов начала и конца в индексном буфере
        std::vector<int> materials_range; // хранилище индексов
        std::vector<int> materials_ids; // индексы материалов

        materials_range.push_back(count); // Закидываем начало отрезка в индексном буфере
        // Цикл по считанным моделям
        for (const auto& shape : shapes) 
        {
            offset = count;  // Переменная для 
            last_material_index = shape.mesh.material_ids[(count - offset)/3]; // Запоминаем индекс материала

            // Цикл по индексам модели
            for (const auto& index : shape.mesh.indices) 
            {
                hash = 0;
                hash_combine( hash
                            , attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2]
                            , attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2]
                            , attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1]);
            
                if (!uniqueVerteces.count(hash))
                {
                    uniqueVerteces[hash] = verteces.size();
                    
                    // группируем вершины в массив на основании индексов
                    verteces.push_back({  attrib.vertices[3 * index.vertex_index + 0]
                                                    , attrib.vertices[3 * index.vertex_index + 1]
                                                    , attrib.vertices[3 * index.vertex_index + 2]
                                                });
                    // группируем нормали в массив на основании индексов
                    normals.push_back({  attrib.normals[3 * index.normal_index + 0]
                                                    , attrib.normals[3 * index.normal_index + 1]
                                                    , attrib.normals[3 * index.normal_index + 2]
                                                });
                    // группируем текстурные координаты в массив на основании индексов
                    texCords.push_back({  attrib.texcoords[2 * index.texcoord_index + 0]
                                                    , 1-attrib.texcoords[2 * index.texcoord_index + 1]
                                                });
                }
                // Сохраняем индекс в массив
                indices.push_back(uniqueVerteces[hash]);
                
                // Если индекс последнего материала изменился, то необходимо сохранить его
                if (last_material_index != shape.mesh.material_ids[(count - offset)/3])
                {
                    materials_range.push_back(count); // как конец отрезка
                    materials_ids.push_back(last_material_index); // как используемый материал
                    last_material_index = shape.mesh.material_ids[(count - offset)/3];
                }
                count++; 
            } // for (const auto& index : shape.mesh.indices) 
            
            // Если последний материал не загружен - загружаем его
            if (materials_range[materials_range.size()-1] != count-1)
            {
                materials_range.push_back(count); // последний конец отрезка
                materials_ids.push_back(last_material_index); // последний используемый материал
            }
        } // for (const auto& shape : shapes) 

        

        // Загрузка в буферы
        model.load_verteces (&verteces[0], verteces.size());
        model.load_normals  (&normals[0],  normals.size());
        model.load_texCoords(&texCords[0], texCords.size());
        // Загрузка индексного буфера
        model.load_indices  (&indices[0],  indices.size());


        // Создаем копии модели, которые будут рендериться в заданном диапазоне
        // И присваиваем текстуры копиям на основании материала
        for (int i = 0; i < materials_range.size()-1; i++)
        {
            result.models.push_back(model); // Создание копии с общим VAO
            auto s = --result.models.end();
            s->set_index_range(materials_range[i]*sizeof(GLuint), materials_range[i+1]-materials_range[i]);

            // Текстуры
            Texture diffuse(TEX_DIFFUSE, texture_directory + materials[materials_ids[i]].diffuse_texname);
            s->set_texture(diffuse);
            Texture ambient(TEX_AMBIENT, texture_directory + materials[materials_ids[i]].ambient_texname);
            s->set_texture(ambient);
            Texture specular(TEX_SPECULAR, texture_directory + materials[materials_ids[i]].specular_texname);
            s->set_texture(specular);

            // Материал
            s->material.ka = pow(glm::vec3(materials[materials_ids[i]].ambient[0],  materials[materials_ids[i]].ambient[1],  materials[materials_ids[i]].ambient[2]), glm::vec3(1/inv_gamma));
            s->material.kd = pow(glm::vec3(materials[materials_ids[i]].diffuse[0],  materials[materials_ids[i]].diffuse[1],  materials[materials_ids[i]].diffuse[2]), glm::vec3(1/inv_gamma));
            s->material.ks = glm::vec3(materials[materials_ids[i]].specular[0], materials[materials_ids[i]].specular[1], materials[materials_ids[i]].specular[2]);
            s->material.p  = (materials[materials_ids[i]].shininess > 0.0f) ? 1000.0f / materials[materials_ids[i]].shininess : 1000.0f;
        }    

        return result;
    }
}