#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int light_up;
uniform float opacity;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec4 base_color = texture(sampler0, texcoord);
	if (light_up == 1) {
		vec4 light_up_color = vec4(1.0,0.0,0.0,1.0) * base_color;	// red tint
		color = mix(base_color, light_up_color, 0.5);
	}
	else {
		color = vec4(fcolor, 1.0) * base_color;
	}
	color.a *= opacity;
}
