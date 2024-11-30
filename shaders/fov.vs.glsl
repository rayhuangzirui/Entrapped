#version 330

// Input attributes
in vec3 in_color;
in vec3 in_position;

out vec3 vcolor;
out vec2 tex_position;
out vec2 screen_position;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	vcolor = in_color;
	tex_position = in_position.xy;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	vec3 wpos = transform * vec3(in_position.xy, 1.0);
	screen_position = wpos.xy;
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}