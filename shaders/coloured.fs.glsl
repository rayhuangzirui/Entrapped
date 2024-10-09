#version 330

in vec3 fcolor;  // Color passed from vertex shader
out vec4 FragColor;

void main()
{
    FragColor = vec4(fcolor, 1.0);  // Output the color with full opacity
}
