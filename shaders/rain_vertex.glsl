#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 3) in mat4 model;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0f);
}
