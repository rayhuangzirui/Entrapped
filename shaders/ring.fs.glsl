#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 tex_position;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(fcolor * vcolor, 1.0);
	float radius = 0.5;
	float thickness = 0.01;
	if (length(tex_position) < radius-thickness || length(tex_position) > radius) {
		discard;
	}
}