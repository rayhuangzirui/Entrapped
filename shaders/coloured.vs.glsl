#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

out vec3 fcolor;

uniform mat3 transform;
uniform mat3 projection;

void main()
{
    gl_Position = vec4(projection * transform * vec3(in_position.xy, 1), 1);
    fcolor = in_color;
}
