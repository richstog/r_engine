#version 420 core 

layout(location = 0) in vec3 pos; 
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 normals; 
layout(location = 3) in vec3 tangent; 
layout(location = 4) in vec3 bitangent; 
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 boneWeights;

layout(std140, binding = 0) uniform Camera3D
{
    mat4 projection;
    mat4 view;
    vec3 position;
} Camera3D;

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

layout(std140, binding = 5) uniform BonesMatrices
{
    mat4 bonesMatrices[100];
};

uniform bool hasBones = false;

uniform sampler2D tex_heights;
uniform float displacement_heightScale = 0.1;

uniform mat4 model;

out vec3 vertex; // Позиция вершины в пространстве
out vec3 N; // Нормаль трансформированноая
out vec2 texCoord; // Текстурные координаты
out vec3 T; // Касательный вектор
out vec3 B; // Бикасательный вектор
out vec3 view; // Вектор от поверхности к камере

void main() 
{ 
    mat4 totalTransform;
    if (!hasBones)
        totalTransform = model;
    else
    {
        totalTransform = mat4(0.0);
        for (int i = 0; i < 4; i++)
        {
            if (boneIds[i] == -1 || boneIds[i] >= 100)
                continue;
            
            totalTransform += bonesMatrices[boneIds[i]] * boneWeights[i];
        }

        totalTransform = totalTransform * model;
    }

    vec4 P = totalTransform * vec4(pos, 1.0); // трансформация вершины
    vertex = P.xyz;

    N = normalize(mat3(totalTransform) * normals); // трансформация нормали

    texCoord = inTexCoord; // Текстурные координаты

    T = normalize(mat3(totalTransform) * tangent);
    B = normalize(mat3(totalTransform) * bitangent);

    view = Camera3D.position - vertex;

    if (displacementmapped)
    {
        float height = texture(tex_heights, texCoord).r * displacement_heightScale;
        P.xyz += mat3(T, B, N) * vec3(0, 0, height);
    }
    
    gl_Position = Camera3D.projection * Camera3D.view * P;
} 