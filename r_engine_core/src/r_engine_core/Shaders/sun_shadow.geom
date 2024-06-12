#version 420 core

layout(triangles, invocations = 4) in; // здесь invocations должно соответствовать количеству каскадов
layout(triangle_strip, max_vertices = 3) out;

layout(std140, binding = 3) uniform Sun
{
    vec3 direction;
    vec3 color;
    mat4 vp[4];
} sun;

void main()
{ 
	for (int i = 0; i < 3; ++i)
	{
		gl_Position = sun.vp[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}  