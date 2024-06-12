#version 330 core

in vec2 texCoord;

out float occlusion;

uniform sampler2D ssao;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssao, 0));
    vec2 offset;
    occlusion = 0.0;
    for (int x = -2; x < 2; x++) 
    {
        for (int y = -2; y < 2; y++) 
        {
            offset = vec2(x, y) * texelSize;
            occlusion += texture(ssao, texCoord + offset).r;
        }
    }
    occlusion = occlusion / (4.0 * 4.0);
}  