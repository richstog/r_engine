#version 420 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

layout(std140, binding = 4) uniform gamma
{
    float inv_gamma;
};

void main()
{    
    FragColor.rgb = pow(texture(skybox, TexCoords).rgb, vec3(inv_gamma));
    gl_FragDepth = 0.9999f;
}