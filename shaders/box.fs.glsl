#version 330

// From Vertex Shader
in vec3 vcolor;

// Application data
uniform vec3 fcolor;
uniform float opacity;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(fcolor * vcolor, 1.0);
	color.r = fcolor.r;
	color.g = fcolor.g;
	color.b = color.b + fcolor.b;
	color.a *= opacity;
}