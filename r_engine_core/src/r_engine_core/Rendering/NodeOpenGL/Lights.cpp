#include "Lights.hpp"

#include "Scene.hpp" // Для отладочного вывода лампочек

#include <stdexcept>

namespace r_engine
{
    GLuint Light::count = 0; // количество используемых источников (должно быть <= MAX_LIGHTS)
    LightData Light::data[MAX_LIGHTS]; // Массив данных по источникам света
    Light Light::lights[MAX_LIGHTS]; // Массив источников-узлов сцены

    Sun Sun::instance; // Экземпляр синглтона
    bool Sun::uploadReq = true; // Необходимость загрузки в следствии изменений

    // возвращает размер буфера в байтах
    int Light::getUBOsize()
    {
        return sizeof(LightData) * MAX_LIGHTS + sizeof(GLuint);
    }

    // Загрузка данных в буфер
    void Light::upload(UBO& lights_data)
    {
        GLuint LightDataSize = sizeof(LightData); // Одного экземпляра структуры LightData
        int first = MAX_LIGHTS, last = -1; // Начало и конец диапазона загрузки источников
        static GLuint prev_count = -1; // Кол-во источников в прошлую посылку

        if (count)
        {
            for (int i = 0; i < MAX_LIGHTS; i++)
            {
                lights[i].recalcMatrices(); // Пересчитаем матрицы по необходимости (проверка внутри метода)

                // Если требуется загрузка
                if (lights[i].uploadReq)
                {
                    lights[i].toData(); // Перевод ноды в данные для шейдера

                    // Определение диапазона загрузки 
                    if (first > lights[i].index)
                        first = lights[i].index;
                    if (last < lights[i].index)
                        last = lights[i].index; 

                    lights[i].uploadReq = false; // Сброс флага
                }
            }
            
            // Если есть что загрузить (определен диапазон)
            if (last > -1)
                lights_data.loadSub(data + first, LightDataSize*(last - first +1), LightDataSize*(first)); // Загрузка данных об источниках
        }

        // Если кол-во изменилось
        if (prev_count != count)
        {
            prev_count = count;

            // Загружаем кол-во источников
            lights_data.loadSub(&count, sizeof(count), LightDataSize*MAX_LIGHTS);
        }
    }

    // Метод пересчета матрицы трансформации по необходимости, должен сбрасывать флаг changed
    void Light::recalcMatrices()
    {
        // Если были изменения - необходимо загрузить данные
        if (changed || parent_changed)
            uploadReq = true;
        
        // Выполняем вычисление матриц методом родительского класса
        Node::recalcMatrices(); 
    }

    // Константный доступ к цвету
    const glm::vec3& Light::c_color() const
    {
        return color;
    }

    // Неконстантная ссылка для изменений цвета
    glm::vec3& Light::e_color()
    {
        uploadReq = true;

        return color;
    }

    // Проверка что не взаимодествуем с пустым источником
    void Light::check_id()
    {
        if (index < 0
        ||  index >= count)
            throw std::runtime_error("Попытка использовать ссылку на пустой или некорректный источник");
    }

    // Преобразует информацию об источнике в структуру LightData
    void Light::toData()
    {
        check_id(); // Проверка на работу с корректным индексом

        // Если позиция изменилась
        if (data[index].position.x != result_transform[3].x
        ||  data[index].position.y != result_transform[3].y
        ||  data[index].position.z != result_transform[3].z
        )
        {
            data[index].position = glm::vec3(result_transform[3]); // Позиция из матрицы трансформации
            recalcVP(); // Пересчет матрицы вида-проекции для расчета теней
        }
        data[index].color = color; // Цвет
        // Если радиус изменился
        if (data[index].attenuation.r != radius)
        {
            data[index].attenuation.r  = radius; // Радиус действия источника
            data[index].attenuation[1] = 4.5/radius;      // Линейный коэф. угасания
            data[index].attenuation[2] = 4 * data[index].attenuation[1] * data[index].attenuation[1]; // Квадратичный коэф. угасания
        }
        // Направление и угол источника
        data[index].direction_angle = glm::vec4( glm::normalize(glm::vec3(result_transform * DEFAULT_LIGHT_DIRECTION))
                                            , angle / 2 // Половинный угол для вычислений на шейдере
                                            );
    }

    // Возвращает ссылку на новый источник света
    Light& Light::getNew() 
    {
        Light& refNew = findByIndex(-1);

        refNew.index = count++;
        refNew.uploadReq = true;

        return refNew;
    }

    // Уничтожает источник света
    void Light::destroy()
    {
        check_id(); // Проверка на работу с корректным индексом
        // Если удаляемый элемент не последний
        if (count-1 != index)
        {
            // Найдем элемент для замены
            Light& replace = findByIndex(--count);

            replace.uploadReq = true; // Требуется загрузить данные
            replace.index = index; // Заменяем индекс данных
        }
        
        operator=(Light()); // Обнулим источник путем замены на новый
    }

    // Возвращает ссылку на источник с нужным индексом
    Light& Light::findByIndex(GLuint index)
    {
        // Если нет источников - возвращаем нулевой
        if (!count)
            return lights[0];

        // Цикл по перебору источников
        for (int i = 0; i < MAX_LIGHTS; i++)
            if (lights[i].index == index)
                return lights[i];

        throw std::runtime_error("Запрашиваемый источник освещения не найден, либо достигнут лимит");
    }

    // Конструктор без параметров
    Light::Light() : Node(), index(-1), uploadReq(false), color(1.0f), radius(10.0f), angle(360.0f)
    {
        
    }

    // Оператор  присваивания
    Light& Light::operator=(const Light& other)
    {
        // Проверка на самоприсваивание
        if (this != &other) 
        {
            index = other.index; // Переносим индекс
            uploadReq = other.uploadReq; // Необходимость загрузки 
            color = other.color;
            radius = other.radius;
            angle = other.angle;

            Node::operator=(other);
        }
        return *this;
    }

    Light::~Light()
    {

    }

    // Рисование отладочных лампочек
    void Light::render(ShaderProgram &shaderProgram, UBO &material_buffer)
    {
        // Загрузка модели лампочки при первом вызове функции
        static Scene bulb = loadOBJtoScene("../resources/models/bulb.obj", "../resources/models/", "../resources/textures/");
        static Model sphere = genShpere(1, 16, &bulb.root);

        GLuint angle_uniform = shaderProgram.getUniformLoc("angle");
        GLuint direction_uniform = shaderProgram.getUniformLoc("direction");

        // Цикл по источникам света
        for (int i = 0; i < count; i++)
        {
            // Загрузим направление
            glUniform3fv(direction_uniform, 1, &data[i].direction_angle.x);
            // Угол для лампочки = 180 (рисуем целую модель)
            glUniform1f(angle_uniform, 180); // Зададим параметры материала сфере действия
            
            // Сдвиг на позицию источника
            bulb.root.e_position() = data[i].position;
            sphere.e_scale() = glm::vec3(data[i].attenuation.r); // Масштабирование сферы
            // Задание цвета
            bulb.models.begin()->material.ka = sphere.material.ka = data[i].color;

            // Вызов отрисовки
            bulb.render(shaderProgram, material_buffer);    

            // Угол для сферы (рисуем направленный конус)
            glUniform1f(angle_uniform, data[i].direction_angle.a); 

            // Рисование сферы покрытия источника в режиме линий
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            sphere.render(shaderProgram, material_buffer);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    // Константный доступ к радиусу
    const float& Light::c_radius() const
    {
        return radius;
    }

    // Неконстантная ссылка для изменений радиуса
    float& Light::e_radius()
    {
        uploadReq = true;

        return radius;
    }

    // Константный доступ к углу освещенности
    const float& Light::c_angle() const
    {
        return angle;
    }

    // Неконстантная ссылка для изменений угла освещенности
    float& Light::e_angle()
    {
        uploadReq = true;

        return angle;
    }

    // Конструктор направленного источника с параметрами направления и цвета
    Sun::Sun(const glm::vec3 &dir, const glm::vec3 &c) : direction(dir), color(c)
    {
        
    }

    // Доступ к синглтону
    Sun& Sun::get()
    {
        return instance;
    }

    // Загрузка данных об источнике на шейдер
    void Sun::upload(UBO& sun_data)
    {
        instance.recalcVP(); // Пересчет матрицы вида-проекции источника по необходимости (влияет на флаг uploadReq)

        if (uploadReq)
        {
            sun_data.loadSub(&instance, sizeof(instance));

            uploadReq = false;
        }
    }

    // Константный доступ к направлению лучей источника
    const glm::vec3& Sun::c_direction() const
    {
        return instance.direction;    
    }

    // Неконстантная ссылка для изменений направления лучей источника
    glm::vec3& Sun::e_direction()
    {
        uploadReq = true;

        return instance.direction;
    }

    // Константный доступ к цвету
    const glm::vec3& Sun::c_color() const
    {
        return instance.color;
    }

    // Неконстантная ссылка для изменений цвета
    glm::vec3& Sun::e_color() 
    {
        uploadReq = true;

        return instance.color;
    }

    // Пересчитывает по необходимости матрицу вида-проекции
    void Sun::recalcVP()
    {
        // Точки по краям проекции камеры
    std::pair <bool, const glm::vec4(*)[8]> camProjCoords = Camera::current().getProjCoords();

        // Есть изменения по источнику или камере
        if (uploadReq || camProjCoords.first)
        {
            uploadReq = true; // Требуется загрузка в следствии пересчета матрицы

            glm::vec3 mean; // Среднее арифметическое
            glm::vec4 max, min; // макс и мин координаты
            glm::vec4 point; // Точка приведенная в пространство источника света

        
            for (int cascade = 0; cascade < CAMERA_CASCADE_COUNT; cascade++)
            {
                mean = glm::vec3(0);
                // Найдем среднее арифметическое от точек для нахождения центра прямоугольника
                for (int i = 0; i < 8; i++)
                    mean += glm::vec3(camProjCoords.second[cascade][i]);
                mean /= 8;
                // Используем среднее арифметическое для получения матрицы вида параллельного источника
                glm::mat4 lightView = glm::lookAt(mean + glm::normalize(direction), mean, CAMERA_UP_VECTOR);
                
                // Примем первую точку как минимальную и максимальную (приведя в пространство вида источника)
                min = max = lightView * camProjCoords.second[cascade][0];
                // Для оставшихся точек
                for (int i = 1; i < 8; i++)
                {
                    // Приведем в пространство вида источника
                    point = lightView * camProjCoords.second[cascade][i];
                    max = glm::max(max, point);
                    min = glm::min(min, point);
                }

                // Максимальное значение глубины
                max.z = std::max(fabs(max.z), fabs(min.z));
                // На основании максимальных и минимальных координат создадим матрицу проекции источника
                vp[cascade] = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z) * lightView;
            }
        }
    }

    // Пересчитывает по необходимости матрицу вида-проекции
    void Light::recalcVP()
    {    
        float near_plane = 0.1f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, radius);
        data[index].vp[0] = shadowProj * glm::lookAt(position, position + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
        data[index].vp[1] = shadowProj * glm::lookAt(position, position + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
        data[index].vp[2] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
        data[index].vp[3] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
        data[index].vp[4] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
        data[index].vp[5] = shadowProj * glm::lookAt(position, position + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    }

    // Возвращает количество источников
    int Light::getCount()
    {
        return count;
    }
}