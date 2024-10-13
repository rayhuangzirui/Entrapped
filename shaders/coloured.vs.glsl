#version 330

layout(location = 0) in vec3 in_position;

uniform mat3 transform;
uniform mat3 projection;

void main()
{
    gl_Position = vec4(projection * transform * vec3(in_position.xy, 1), 1);
}