// GLSL
#version 450

layout(location = 0) in vec3 vertColor;
layout(location = 0) out vec4 fragColor;

void main_fs()
{
	fragColor = vec4(vertColor, 1.0f);
}