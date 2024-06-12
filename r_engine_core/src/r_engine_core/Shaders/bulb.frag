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

in vec3 pos_local;

layout(std140, binding = 4) uniform gamma
{
    float inv_gamma;
};

layout (location = 1) out vec3 gNormal;
layout (location = 4) out uvec3 gID;
layout (location = 5) out vec3 gEmittedColor;

uniform float angle;
uniform vec3 direction;

uniform uvec3 ID = uvec3(0);

void main()
{   
    float cosA = dot(normalize(pos_local), normalize(direction));
    if (degrees(acos(cosA)) <= angle)
        gEmittedColor = pow(base_color, vec3(inv_gamma));
    else
        discard;

    gNormal = vec3(0);
    
    // Сохранение идентификатора объекта
    gID = ID;
}