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

layout (location = 1) out vec3 gNormal;
layout (location = 4) out uvec3 gID;
layout (location = 5) out vec3 gEmittedColor;

in vec3 vertex; // Позиция вершины в пространстве
in vec3 N; // Нормаль трансформированная
in vec2 texCoord; // Текстурные координаты
in vec3 T; // Касательный вектор
in vec3 B; // Бикасательный вектор
in vec3 view; // Вектор от поверхности к камере

uniform float parallax_heightScale = 0.1;

uniform uvec3 ID = uvec3(0);

void main()
{    
    gNormal = vec3(0);
    // Сохранение базового цвета в качестве излучаемого
    gEmittedColor = base_color;
    // Сохранение идентификатора объекта
    gID = ID;
    
    gl_FragDepth = 0.01 * gl_FragCoord.z;
}