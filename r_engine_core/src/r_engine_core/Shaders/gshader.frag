#version 420 core 

layout(std140, binding = 1) uniform Material
{
    vec3 base_color;
    float roughness;
    float metallic;
    float specular;
    vec3 emitted;
    bool normalmapped;
    bool parallaxmapped;
    bool displacementmapped;
};

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gBaseColor;
layout (location = 3) out vec3 gRMS;
layout (location = 4) out uvec3 gID;
layout (location = 5) out vec3 gEmittedColor;

in vec3 vertex; // Позиция вершины в пространстве
in vec3 N; // Нормаль трансформированноая
in vec2 texCoord; // Текстурные координаты
in vec3 T; // Касательный вектор
in vec3 B; // Бикасательный вектор
in vec3 view; // Вектор от поверхности к камере

uniform sampler2D tex_albedo;
uniform sampler2D tex_roughness;
uniform sampler2D tex_metallic;
uniform sampler2D tex_specular;
uniform sampler2D tex_emitted;
uniform sampler2D tex_heights;
uniform sampler2D tex_normal;

uniform float parallax_heightScale = 0.1;

uniform uvec3 ID = uvec3(0);

void main()
{    
    // Сформируем TBN матрицу
    mat3 TBN = mat3(T, B, N);
    // Перевод вектора в касательное пространство
    vec3 viewTBN = normalize(transpose(TBN) * view);
    // Измененные текстурные координаты
    vec2 new_texCoord = texCoord;

    // Сохранение позиции фрагмента в G-буфере
    gPosition = vertex;

    if (parallaxmapped)
    {
        // Число слоев
        float layersCount = 32;  
        // Вычислим размер каждого слоя
        float layerDepth = 1.0 / layersCount;
        // Глубина текущего слоя
        float currentLayerDepth = 0.0;
        // Величина сдвига между слоями
        vec2 deltaTexCoords = (parallax_heightScale * viewTBN.xy / viewTBN.z) / layersCount;
    
        // Переменные для вычислений
        vec2  currentTexCoords     = texCoord;
        float currentDepthMapValue = 1.0 - texture(tex_heights, currentTexCoords).r;
        
        // Пока глубина текущего слоя меньше текущего значения глубины из текстуры 
        while(currentLayerDepth < currentDepthMapValue)
        {
            // Сдвигаем координаты
            currentTexCoords -= deltaTexCoords;
            // Обновляем значение глубины из текстуры
            currentDepthMapValue = 1.0 - texture(tex_heights, currentTexCoords).r;  
            // Сдвигаем глубину на следующий слой
            currentLayerDepth += layerDepth;  
        }
        
        // Получим значение текстурных координат с предыдущего шага
        vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

        // Значения глубины до и после пересечения
        float afterDepth  = currentDepthMapValue - currentLayerDepth;
        float beforeDepth = 1.0 - texture(tex_heights, prevTexCoords).r - currentLayerDepth + layerDepth;
    
        // Интерполяция текстурных координат
        float weight = afterDepth / (afterDepth - beforeDepth);
        new_texCoord = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

        // Проверка диапазона [0;1]
        if(new_texCoord.x > 1.0 || new_texCoord.y > 1.0 || new_texCoord.x < 0.0 || new_texCoord.y < 0.0)
            discard;
    }

    // Сохранение нормали в G-буфере
    gNormal = N;
    // Если используется карта нормалей
    if (normalmapped)
    {
        // Получим значение из карты нормалей и приведем их к диапазону [-1;1]
        gNormal = texture(tex_normal, new_texCoord).rgb * 2 - 1.0f;  
        gNormal = normalize(TBN * gNormal); // Из касательного пространства в мировые координаты
    }

    // Сохранение базового цвета
    gBaseColor.rgb = base_color.r<0?texture(tex_albedo, new_texCoord).rgb:base_color;
    // Если используется двухканальная текстура
    if (roughness < -1)
    {
        // Сохранение шероховатости и металличности
        gRMS.rg = texture(tex_metallic, new_texCoord).bg;
    }
    else
    {
        // Сохранение шероховатости
        gRMS.r = roughness<0?texture(tex_roughness, new_texCoord).r:roughness;
        // Сохранение металличности
        gRMS.g = metallic<0?texture(tex_metallic, new_texCoord).r:metallic;
    }
    // Сохранение интенсивности блика диэлектриков
    gRMS.b = specular<0?texture(tex_specular, new_texCoord).r:specular;
    // Сохранение идентификатора объекта
    gID = ID;
    // Сохранение излучаемого света
    gEmittedColor.rgb = emitted.r<0?texture(tex_emitted, new_texCoord).rgb:emitted;
}