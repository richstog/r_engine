#version 420 core

layout (location = 0) in vec3 pos;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 boneWeights;

layout(std140, binding = 5) uniform BonesMatrices
{
    mat4 bonesMatrices[100];
};

uniform bool hasBones = false;

uniform mat4 model;

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
    gl_Position = totalTransform * vec4(pos, 1.0);
}
