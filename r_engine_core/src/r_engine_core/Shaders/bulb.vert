#version 420 core 

layout(location = 0) in vec3 pos; 

layout(std140, binding = 0) uniform Camera3D
{
    mat4 projection;
    mat4 view;
    vec3 position;
} Camera3D;

uniform mat4 model;

out vec3 pos_local;

void main() 
{ 
    pos_local = pos;
    gl_Position = Camera3D.projection * Camera3D.view * model * vec4(pos, 1.0);
} 