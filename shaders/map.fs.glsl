#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;

// opacity
uniform float opacity;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
    color = texture(sampler0, texcoord);
    color = color * opacity;
}

