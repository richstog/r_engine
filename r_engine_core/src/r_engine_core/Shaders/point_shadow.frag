#version 330 core

in vec4 FragPos;
in vec3 lightPos;
in float radius;

void main()
{
    // Расстояние между источником и фрагментом
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // Приведение к диапазону [0;1]
    lightDistance = lightDistance / radius;
    
    // Замена значения глубины
    gl_FragDepth = lightDistance;
}