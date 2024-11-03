#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 t
uniform mat3 projection;
uniform float xpos;
uniform float ypos;
uniform float w_ratio;
uniform float h_ratio;

void main()
{
	vec2 t = in_texcoord;
	// t.x = t.x * 2.0;
	// if (t.x > 1.0){
		// t.x = 0.0;
	// }
	texcoord = t;

	vec3 a = in_position;
	a.x = (a.x+0.5)*w_ratio - 0.5;
	a.y = (a.y-0.5)*h_ratio + 0.5;
	vec3 p = transform * vec3(a.xy, 1.0);
	 p.x = p.x+xpos;
	 p.y = p.y+ypos;
	vec3 pos = projection * p;
	// pos.x = pos.x + 1.0;
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}