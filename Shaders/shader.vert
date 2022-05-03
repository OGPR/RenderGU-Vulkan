// GLSL
#version 450

layout(location = 0) out vec3 vertColor; 

// TODO pass in vertex buffer (data)

vec3 positions[3] = vec3[]
(
	vec3(0.0f, -0.4f, 0.0),
	vec3(0.4f, 0.4f, 0.0),
	vec3(-0.4f, 0.4f, 0.0)
);

vec3 colors[3] = vec3[]
(
	vec(1.0f, 0.0f, 0.0f),
	vec(0.0f, 1.0f, 0.0f),
	vec(0.0f, 0.0f, 1.0f),
)

// This can be named anything
void main_vs()
{
	gl_Position = vec4(positions[gl_VertexIndex], 1.0f); // gl_VertexIndex will keep track of where we are in our position info
	vertColor = colors[gl_VertexIndex];
}