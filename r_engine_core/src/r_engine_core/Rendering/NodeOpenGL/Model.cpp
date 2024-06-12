#include "Model.hpp"

#include <algorithm>

namespace r_engine
{
    // Конструктор с заданным родителем (по умолчанию NULL)
    Node::Node(Node* parent_) : parent(parent_), result_transform(1), parent_changed(false),
    position(0), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1), changed(false), transform(1)
    {
        if (parent)
        {
            // Запишем себя в потомки
            parent->children.push_back(this);
            parent_changed = true;
        }
    } 

    // Конструктор копирования
    Node::Node(const Node& copy): position(copy.position), rotation(copy.rotation), scale(copy.scale),
    parent(copy.parent), changed(copy.changed), parent_changed(copy.parent_changed), transform(1), result_transform(1)
    {
        // Запишем себя в потомки
        if (parent)
            parent->children.push_back(this);
        // Если у оригинала не было изменений - перепишем матрицу трансформации
        if (!changed)
            transform = copy.transform;
        // Если у родителя не было изменений для оригинала - перепишем результирующую матрицу трансформации
        if (!parent_changed)
            result_transform = copy.result_transform;
    }

    Node::~Node()
    {
        setParent(NULL); // Удаляем себя из потомков
        // Сообщаем потомкам об удалении родителя
        for (Node* child : children)
            child->setParent(NULL);
    }

    // Возвращает необходимость пересчета матрицы трансформации
    bool Node::isChanged() 
    { 
        return changed; 
    } 

    // Константный доступ к позиции
    const glm::vec3& Node::c_position() const 
    { 
        return position; 
    } 

    // Константный доступ к повороту
    const glm::quat& Node::c_rotation() const 
    { 
        return rotation; 
    } 

    // Константный доступ к масштабированию
    const glm::vec3& Node::c_scale() const 
    { 
        return scale; 
    } 

    // Неконстантная ссылка для изменений позиции
    glm::vec3& Node::e_position() 
    { 
        changed = true; // Флаг о изменении
        invalidateParent(); // Проход потомков в глубину с изменением флага parent_changed
        return position; 
    } 

    // Неконстантная ссылка для изменений поворота
    glm::quat& Node::e_rotation() 
    { 
        changed = true; // Флаг о изменении
        invalidateParent(); // Проход потомков в глубину с изменением флага parent_changed
        return rotation; 
    } 

    // Неконстантная ссылка для изменений масштабирования         
    glm::vec3& Node::e_scale() 
    { 
        changed = true; // Флаг о изменении
        invalidateParent(); // Проход потомков в глубину с изменением флага parent_changed
        return scale; 
    } 

    // Возвращает матрицу трансформации модели
    const glm::mat4& Node::getTransformMatrix() 
    {
        // Если требуется - пересчитаем матрицу
        recalcMatrices();
            
        return result_transform;
    }

    // Пересчет матрицы трансформации модели, если это требуется
    void Node::recalcMatrices() 
    {
        // Если было изменение по векторам позиции, поворота и масштабирования
        if  (changed)
        {
            transform = glm::mat4(1.0f);
            // Перемещение модели
            transform = glm::translate(transform, position);
            // Поворот модели
            transform = transform * glm::mat4_cast(rotation);
            // Масштабирование
            transform = glm::scale(transform, scale);  
        }

        // Если собственная или родительская матрицы менялись - необходимо пересчитать итоговую
        if (changed || parent_changed)
        {
            if (parent) // Если есть родитель
                result_transform = parent->getTransformMatrix() * transform;
            else // Если нет родителя
                result_transform = transform;

            parent_changed = changed = false; // Изменения применены
        }
    } 

    // Проход потомков в глубину с изменением флага parent_changed
    void Node::invalidateParent()
    {
        // Цикл по потомкам
        for (Node* child : children)
        {
            child->parent_changed = true; // Флаг 
            child->invalidateParent(); // Рекурсивный вызов для потомков выбранного потомка
        }
    }

    // Устанавливает родителя для узла
    void Node::setParent(Node * parent)
    {
        // Если замена происходит на другого родителя
        if (parent != this->parent)
        {
            Node* tmp = parent;
            // Проверка на зацикливание об самого себя
            while (tmp)
            {
                if (tmp == this)
                    return; // Можно выдать exception
                tmp = tmp->parent;
            }
            // Если есть старый родитель - удалим себя из его потомков
            if (this->parent)
            {
                // Поиск в списке родительских потомков
                auto position = std::find(this->parent->children.begin(), this->parent->children.end(), this);
                // Если итератор указывает в конец - ничего не найдено
                if (position != this->parent->children.end()) 
                    this->parent->children.erase(position); // Само удаление
            }
            
            this->parent = parent; // Заменяем указатель на родителя
            // Если родитель не NULL - добавляем себя в детей
            if (parent) 
                parent->children.push_back(this);
            // В любом случае необходимо пересчитать собственную итоговую матрицу
            parent_changed = true;
        }
    }

    // Возвращает указатель на родителя
    Node* Node::getParent() 
    { 
        return parent; 
    }

    // Возвращает ссылку на вектор дочерних узлов
    const std::vector<Node*>& Node::getChildren() const 
    { 
        return children; 
    }

    // Оператор присваивания
    Node& Node::operator=(const Node& other)
    {
        position = other.position;
        rotation = other.rotation;
        scale = other.scale;
        changed = other.changed;

        if (!changed)
            transform = other.transform;
            
        setParent(other.parent);
        
        // Если у other флаг parent_changed == false, то можно переписать матрицу результата с него
        if (!other.parent_changed)
        {
            result_transform = other.result_transform;
            parent_changed = false; // Сбрасываем флаг после смены родителя
        }

        return *this;
    }

    // Конструктор по умолчанию
    Model::Model(Node *parent) : Node(parent), verteces_count(0), first_index_byteOffset(0), indices_count(0), 
    vertex_vbo(VERTEX), index_vbo(ELEMENT), normals_vbo(VERTEX), texCoords_vbo(VERTEX)
    {

    }

    // Конструктор копирования
    Model::Model(const Model& copy) : Node(copy),
    vao(copy.vao), 
    verteces_count(copy.verteces_count), first_index_byteOffset(copy.first_index_byteOffset), indices_count(copy.indices_count), 
    vertex_vbo(copy.vertex_vbo), index_vbo(copy.index_vbo), normals_vbo(copy.normals_vbo), texCoords_vbo(copy.texCoords_vbo),
    texture_diffuse(copy.texture_diffuse), texture_ambient(copy.texture_ambient), texture_specular(copy.texture_specular), 
    material(copy.material)
    {

    }

    // Оператор присваивания
    Model& Model::operator=(const Model& other)
    {
        Node::operator=(other); // Явный вызов родительского оператора копирования
        
        vao = other.vao; 
        verteces_count = other.verteces_count;
        first_index_byteOffset = other.first_index_byteOffset;
        indices_count = other.indices_count;
        
        vertex_vbo = other.vertex_vbo;
        index_vbo = other.index_vbo;
        texCoords_vbo = other.texCoords_vbo;

        texture_diffuse = other.texture_diffuse;
        texture_ambient = other.texture_ambient;
        texture_specular = other.texture_specular;
        
        material = other.material;

        return *this;
    }

    Model::~Model()
    {

    }

    // Вызов отрисовки без uniform-данных
    void Model::render()
    {
        // Подключаем VAO
        vao.use();
        // Если есть индексы - рисуем с их использованием
        if (indices_count)
        {
            index_vbo.use();
            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, (void*)(first_index_byteOffset));
        }
        // Если есть вершины - рисуем на основании массива вершин
        else if (verteces_count)
            glDrawArrays(GL_TRIANGLES, 0, verteces_count);
    }

    // Вызов отрисовки
    void Model::render(ShaderProgram &shaderProgram, UBO &material_buffer) 
    {
        // Расчитаем матрицу трансформации
        glUniformMatrix4fv(shaderProgram.getUniformLoc("model"), 1, GL_FALSE, &this->getTransformMatrix()[0][0]);

        // Подключаем текстуры
        texture_diffuse.use();
        texture_ambient.use();
        texture_specular.use();
        

        // Загружаем данные о материале
        material_buffer.load(&material, sizeof(material));

        render();
    }

    // Функция для конфигурации атрибута вершинного буфера
    void vertex_attrib_config()
    {
        // Определим спецификацию атрибута
        glVertexAttribPointer(  0         // индекс атрибута, должен совпадать с Layout шейдера
                            , 3         // количество компонент одного элемента
                            , GL_FLOAT  // тип
                            , GL_FALSE  // необходимость нормировать значения
                            , 0         // шаг
                            , (void *)0 // отступ с начала массива
                            );
        // Включаем необходимый атрибут у выбранного VAO
        glEnableVertexAttribArray(0);
    }

    // Загрузка вершин в буфер
    void Model::load_verteces(glm::vec3* verteces, GLuint count)
    {
        // Подключаем VAO и вершинный буфер
        vao.use();
        vertex_vbo.use();

        // Загрузка вершин в память буфера
        vertex_vbo.load(verteces, sizeof(glm::vec3)*count);
        vertex_attrib_config();
        // Запоминаем количество вершин для отрисовки
        verteces_count = count;
    }

    // Загрузка индексов в буфер
    void Model::load_indices(GLuint* indices, GLuint count) 
    {
        // Подключаем VAO и индексный буфер
        vao.use();
        index_vbo.use();

        // Загрузка вершин в память буфера
        index_vbo.load(indices, sizeof(GLuint)*count);
        // Запоминаем количество вершин для отрисовки
        indices_count = count;
    }

    // Функция для конфигурации атрибута вершинного буфера
    void texCoords_attrib_config()
    {
        // Определим спецификацию атрибута
        glVertexAttribPointer(  1         // индекс атрибута, должен совпадать с Layout шейдера
                            , 2         // количество компонент одного элемента
                            , GL_FLOAT  // тип
                            , GL_FALSE  // необходимость нормировать значения
                            , 0         // шаг
                            , (void *)0 // отступ с начала массива
                            );
        // Включаем необходимый атрибут у выбранного VAO
        glEnableVertexAttribArray(1);
    } 

    // Загрузка текстурных координат в буфер
    void Model::load_texCoords(glm::vec2* texCoords, GLuint count)
    {
        // Подключаем VAO
        vao.use();

        texCoords_vbo.use();

        // Загрузка вершин в память буфера
        texCoords_vbo.load(texCoords, sizeof(glm::vec2)*count);
        texCoords_attrib_config();
    }

    // Функция для конфигурации атрибута вершинного буфера
    void normals_attrib_config()
    {
        // Устанавливаем связь между VAO и привязанным VBO
        glVertexAttribPointer(  2         // индекс атрибута, должен совпадать с Layout шейдера
                            , 3         // количество компонент одного элемента
                            , GL_FLOAT  // тип
                            , GL_FALSE  // необходимость нормировать значения
                            , 0         // шаг
                            , (void *)0 // отступ с начала массива
                            );
        // Включаем необходимый атрибут у выбранного VAO
        glEnableVertexAttribArray(2);
    } 

    // Загрузка нормалей в буфер
    void Model::load_normals(glm::vec3* normals, GLuint count) 
    {
        // Подключаем VAO
        vao.use();

        normals_vbo.use();

        // Загрузка вершин в память буфера
        normals_vbo.load(normals, sizeof(glm::vec3)*count);
        normals_attrib_config();
    }

    // Ограничение диапазона из буфера индексов
    void Model::set_index_range(size_t first_byteOffset, size_t count)
    {
        first_index_byteOffset = first_byteOffset;
        indices_count = count;
    } 

    // Привязка текстуры к модели
    void Model::set_texture(Texture& texture)
    {
        GLuint type = texture.getType();
        switch(type)
        {
            case TEX_DIFFUSE:
                texture_diffuse = texture;
                break;
            case TEX_AMBIENT:
                texture_ambient = texture;
                break;
            case TEX_SPECULAR:
                texture_specular = texture;
                break;
        };
    }

    // Генерирует сферу заданного радиуса с определенным количеством сегментов
    Model genShpere(float radius, int sectorsCount, Node* parent)
    {
        Model result(parent);

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<GLuint> indices;

        float x, y, z, xy; // Позиция вершины
        float nx, ny, nz, lengthInv = 1.0f / radius; // Нормаль вершины
        float PI = 3.14159265;
        float sectorStep = PI / sectorsCount; // Шаг сектора
        float longAngle, latAngle; // Углы

        for(int i = 0; i <= sectorsCount; ++i)
        {
            latAngle = PI / 2 - i * sectorStep; // Начиная с pi/2 до -pi/2
            xy = radius * cos(latAngle); // r * cos(lat)
            z = radius * sin(latAngle); // r * sin(lat)

            // добавляем (sectorCount+1) вершин на сегмент
            // Последняя и первая вершины имеют одинаковые нормали и координаты
            for(int j = 0; j <= sectorsCount; ++j)
            {
                longAngle = j * 2 * sectorStep; // Начиная с 0 до 2*pi

                // Положение вершины (x, y, z)
                x = xy * cos(longAngle); // r * cos(lat) * cos(long)
                y = xy * sin(longAngle); // r * cos(lat) * sin(long)
                vertices.push_back({x, y, z});

                // Нормали (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                normals.push_back({nx, ny, nz});
            }
        }
        int k1, k2;
        for(int i = 0; i < sectorsCount; ++i)
        {
            k1 = i * (sectorsCount + 1); // начало текущего сегмента
            k2 = k1 + sectorsCount + 1; // начало следующего сегмента

            for(int j = 0; j < sectorsCount; ++j, ++k1, ++k2)
            {
                // 2 треугольника на один сегмент
                // k1, k2, k1+1
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1, k2, k2+1
                if(i != (sectorsCount-1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }

            }
        }
        // Загрузка в модель
        result.load_verteces(&vertices[0], vertices.size());
        result.load_normals(&normals[0], normals.size());
        result.load_indices(&indices[0], indices.size());

        return result;
    }
}