#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler0, texcoord).r);
    color = vec4(fcolor, 1.0) * sampled;
}
