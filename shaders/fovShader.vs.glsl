#version 330 core
layout(location = 0) in vec3 in_position;

uniform mat3 projection;
uniform mat3 view;
uniform mat3 transform;

void main() {
    gl_Position = vec4(in_position.xy, 0.0, 1.0);
}