#version 420 core
layout (triangles, invocations = 6) in; // здесь invocations соответствует числу сторон кубической карты теней
layout (triangle_strip, max_vertices=18) out; // здесь max_vertices = 3 вершины * 6 вызовов на стороны куба

struct LightData
{
    vec3 position;
    vec3 color;
    vec3 attenuation;
    vec4 direction_angle;
    mat4 vp[6];
};

layout(std140, binding = 2) uniform Light
{
    LightData data[64];
    int count;
} light_g;

uniform int light_i;

out vec4 FragPos; 
out vec3 lightPos;
out float radius;

void main()
{
    for(int i = 0; i < 3; ++i)
    {
        FragPos = gl_in[i].gl_Position;
        lightPos = light_g.data[light_i].position;
        radius = light_g.data[light_i].attenuation.r;
        gl_Position = light_g.data[light_i].vp[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID + light_i*6; 
        EmitVertex();
    }    
    EndPrimitive();
} 