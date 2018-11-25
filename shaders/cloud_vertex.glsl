#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in float color;
layout (location = 3) in mat4 instanceOffset;

uniform mat4 view;
uniform mat4 projection;

out float vColor;

void main()
{   
    mat4 model = instanceOffset;
    gl_Position = projection * view * model * vec4(vertex, 1.0);

    vColor = color;
}