#version 420 core

in vec2 texCoord;

out float occlusion;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noise;

layout(std140, binding = 0) uniform Camera3D
{
    mat4 projection;
    mat4 view;
    vec3 position;
} Camera3D;

layout(std140, binding = 3) uniform SSAO
{
    float radius;
    float bias;
    int size;
    vec2 scale;
    vec3 samples[64];
} ssao;

void main()
{
    // Получим информацию из текстур для данного фрагмента по текстурным координатам
    vec3 fragPos = (Camera3D.view * vec4(texture(gPosition, texCoord).xyz, 1)).xyz;
    vec3 normal = normalize(texture(gNormal, texCoord).rgb);
    vec3 randomVec = normalize(texture(noise, texCoord * ssao.scale).xyz);
    // Расчет TBN матрицы
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float sampleDepth; // Значение глубины образца выборки
    vec3 samplePos; // Выборка, ориентированная в пространстве вида камеры
    vec4 sampleCoord; // Выборка, преобразованная к текстурным координатам
    float rangeCheck; // Проверка диапазона
    
    // Проинициализируем значение счетчика и запустим цикл по выборкам
    occlusion = 0;
    for(int i = 0; i < ssao.size; i++)
    {
        samplePos = TBN * ssao.samples[i]; // в TBN-пространстве
        samplePos = fragPos + samplePos * ssao.radius; // в пространстве вида камеры
        
        sampleCoord = Camera3D.projection * vec4(samplePos, 1.0); 
        sampleCoord.xyz /= sampleCoord.w; // Деление на значение перспективы
        sampleCoord.xyz = sampleCoord.xyz * 0.5 + 0.5; // Трансформация в диапазон [0.0; 1.0]
        
        // Получаем значение глубины по образцу выборки
        sampleDepth = (Camera3D.view * vec4(texture(gPosition, sampleCoord.xy).rgb, 1)).z; 

        rangeCheck = smoothstep(0.0, 1.0, ssao.radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + ssao.bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1 - (occlusion / ssao.size);
}
