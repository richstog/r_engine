#version 420 core 

layout(location = 0) in vec3 pos; 

out vec2 texCoord;

void main() 
{ 
    gl_Position = vec4(pos, 1.0);
    texCoord = (pos.xy + vec2(1.0)) / 2; // Переход от [-1;1] к [0;1]
}