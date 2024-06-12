#include "Camera.hpp"

namespace r_engine
{
    // Указатель на текущую используемую камеру
    Camera* Camera::p_current = NULL; 

    // Границы каскадов
    const float camera_cascade_distances[] = {CAMERA_NEAR, CAMERA_FAR / 50.0f, CAMERA_FAR / 10.0f,  CAMERA_FAR / 3.0f, CAMERA_FAR};

    // Защищенный (protected) конструктор камеры без перспективы 
    Camera::Camera(const glm::vec3 &pos, const glm::vec3 &initialRotation) : Node(NULL) // Пусть по умолчанию камера не относится к сцене
    {
        sensitivity = CAMERA_DEFAULT_SENSIVITY;
        position = pos; // задаем позицию
        // Определяем начальный поворот
        glm::quat rotationAroundX = glm::angleAxis( glm::radians(initialRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat rotationAroundY = glm::angleAxis(-glm::radians(initialRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat rotationAroundZ = glm::angleAxis( glm::radians(initialRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation = rotationAroundX * rotationAroundY * rotationAroundZ;
        // Признак изменения
        changed = true;
    }

    // Конструктор камеры с проекцией перспективы
    Camera::Camera(float aspect, const glm::vec3 &position, const glm::vec3 &initialRotation, float fovy)
    : Camera(position, initialRotation)
    {
        setPerspective(fovy, aspect);
    }

    // Конструктор ортографической камеры
    Camera::Camera(float width, float height, const glm::vec3 &position, const glm::vec3 &initialRotation)
    : Camera(position, initialRotation)
    {
        setOrtho(width, height);
    }

    // Конструктор копирования камеры
    Camera::Camera(const Camera& copy) 
    : Node(copy), projection(copy.projection), requiredRecalcVP(copy.requiredRecalcVP), sensitivity(copy.sensitivity),
    requiredRecalcCoords(true)
    {
        // Если у оригинала не было изменений - перепишем матрицу вида-проекции
        if (!requiredRecalcVP)
            vp = copy.vp;
    }

    // Оператор присваивания
    Camera& Camera::operator=(const Camera& other)
    {
        Node::operator=(other); // Вызов родительского оператора= для переноса узла
        
        projection = other.projection;
        requiredRecalcVP = other.requiredRecalcVP;
        sensitivity = other.sensitivity;

        // Если у оригинала не было изменений - перепишем матрицу вида-проекции
        if (!requiredRecalcVP)
            vp = other.vp;

        return *this;
    }

    // Деструктор
    Camera::~Camera() 
    { 
        if (p_current == this)
            p_current = NULL;
    }

    // Возвращает ссылку на константную матрицу проекции
    const glm::mat4& Camera::getProjection()
    {
        return projection;
    }

    // Возвращает ссылку на константную матрицу вида
    const glm::mat4& Camera::getView()
    {
        recalcMatrices();

        return view;
    }

    // Возвращает ссылку на константную матрицу вида
    const glm::mat4& Camera::getVP()
    {
        recalcMatrices();

        return vp;
    }

    // Устанавливает заданную матрицу перспективы
    void Camera::setPerspective(float fovy, float aspect)
    {
        projection = glm::perspective(glm::radians(fovy), aspect, CAMERA_NEAR, CAMERA_FAR);
        requiredRecalcVP = true;
        for (int cascade = 0; cascade < CAMERA_CASCADE_COUNT; cascade++)
            cascade_proj[cascade] = glm::perspective(glm::radians(fovy), aspect, camera_cascade_distances[cascade], camera_cascade_distances[cascade+1]);
    }

    // Устанавливает заданную ортографическую матрицу
    void Camera::setOrtho(float width, float height)
    {
        const float aspect = width / height;
        projection = glm::ortho(-1.0f, 1.0f, -1.0f/aspect, 1.0f/aspect, CAMERA_NEAR, CAMERA_FAR);
        requiredRecalcVP = true;
        for (int cascade = 0; cascade < CAMERA_CASCADE_COUNT; cascade++)
            cascade_proj[cascade] = glm::ortho(-1.0f, 1.0f, -1.0f/aspect, 1.0f/aspect, camera_cascade_distances[cascade], camera_cascade_distances[cascade+1]);

    }

    // Изменяет чувствительность мыши
    void Camera::setSensitivity(float sens)
    {
        sensitivity = sens;
    }

    // Возвращает чувствительность мыши
    const float& Camera::getSensitivity() const
    {
        return sensitivity; 
    }

    // Метод пересчета матрицы вида и произведения Вида*Проекции по необходимости, должен сбрасывать флаг changed
    void Camera::recalcMatrices()
    {
        if (changed || parent_changed)
        {
            glm::vec3 _position = position;
            glm::quat _rotation = rotation;
            if (parent) // Если есть родитель
            {
                glm::mat4 normalized_transform = parent->getTransformMatrix();
                for (int i = 0; i < 3; i++) 
                {
                    glm::vec3 axis = glm::vec3(normalized_transform[i]);
                    normalized_transform[i] = glm::vec4(glm::normalize(axis), normalized_transform[i].w);
                }
                glm::vec4 tmp = normalized_transform * glm::vec4(_position, 1.0f);
                tmp /= tmp.w;
                _position = glm::vec3(tmp);
                _rotation = glm::quat_cast(normalized_transform) * _rotation;
            }
            glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(_rotation));
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -_position);
            view = rotationMatrix * translationMatrix;
            requiredRecalcVP = true;
        }
    
        Node::recalcMatrices();

        if (requiredRecalcVP)
        {
            vp = projection * view;
            requiredRecalcCoords = true; // Требуется пересчитать точки пространства камеры
            requiredRecalcVP = false; // Изменения применены
        }
    }

    // Поворачивает камеру на dx и dy пикселей с учетом чувствительности
    void Camera::rotate(const glm::vec2 &xyOffset)
    {
        // xyOffset - сдвиги координат мыши, xyOffset.x означает поворот вокруг оси Y, а xyOffset.y - поворот вокруг оси X
        
        // Вращение вокруг оси Y
        glm::quat qY = glm::angleAxis(-xyOffset.x * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Вращение вокруг оси X
        glm::quat qX = glm::angleAxis(xyOffset.y * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f));

        // Сначала применяем вращение вокруг Y, затем вокруг X
        rotation = qY * rotation * qX;

        changed = true;
        invalidateParent(); // Проход потомков в глубину с изменением флага parent_changed
    }

    // Использование этой камеры как текущей
    void Camera::use()
    {
        p_current = this;
    }

    // Ссылка на текущую используемую камеру
    Camera& Camera::current()
    {
        static Camera default_cam(800.0f/600.0f);

        if (!p_current)
            return default_cam;
        else
            return *p_current;
    }

    // Данные о камере для шейдера
    CameraData& Camera::getData()
    {
        static CameraData data;
        data = {getProjection(), getView(), position};
        return data;
    }

    // Доступ к координатам с флагом изменения, описывающим пространство вида с пересчетом, если это требуется
    std::pair<bool, const glm::vec4(*)[8]> Camera::getProjCoords() 
    {
        const glm::mat4& cam_vp = getVP(); // Получение ссылки на матрицу вида-проекции с пересчетом, если требуется и активацией флага requiredRecalcCoords
        bool changes = false; // Возвращаемое значение

        if (requiredRecalcCoords)
        { 
            // Инверсия матрицы вида/проекции камеры
            glm::mat4 inv = glm::inverse(cam_vp);
            // Типовые точки, описывающие пространство 
            glm::vec4 typical_points[8] = {  { 1, 1, 1,1}
                                        , { 1, 1,-1,1}
                                        , { 1,-1, 1,1}
                                        , { 1,-1,-1,1}
                                        , {-1, 1, 1,1}
                                        , {-1, 1,-1,1}
                                        , {-1,-1, 1,1}
                                        , {-1,-1,-1,1}};
            
            for (int cascade = 0; cascade < CAMERA_CASCADE_COUNT; cascade++)
            {
                glm::mat4 inv = glm::inverse(cascade_proj[cascade] * getView());
                // Цикл по типовым точкам
                for (int i = 0; i < 8; i++)
                {
                    coords[cascade][i] = inv * typical_points[i];
                    coords[cascade][i] /= coords[cascade][i].w;
                } 
            }

            requiredRecalcCoords = false; // Сбрасываем флаг
            changes = true;
        }

        return std::make_pair(changes, coords); 
    }
}