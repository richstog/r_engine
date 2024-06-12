#version 420 core
layout (location = 0) in vec3 pos;

out vec3 TexCoords;

layout(std140, binding = 0) uniform Camera3D
{
    mat4 projection;
    mat4 view;
    vec3 position;
} Camera3D;

void main()
{
    TexCoords = pos;
    gl_Position = Camera3D.projection * mat4(mat3(Camera3D.view)) * vec4(pos, 1.0);
}  