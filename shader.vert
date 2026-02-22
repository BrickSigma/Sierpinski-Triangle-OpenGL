#version 330

layout (location=0) in vec3 coord;
layout (location=1) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec3 outColor;

void main()
{
    gl_Position = perspective * view * model * vec4(coord.xyz, 1.0);
    outColor = color;
}