#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float xpos;
uniform float ypos;

void main()
{
	texcoord = in_texcoord;
	vec3 p = transform * vec3(in_position.xy, 1.0);
	 p.x = p.x+xpos;
	 p.y = p.y+ypos;
	vec3 pos = projection * p;
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}