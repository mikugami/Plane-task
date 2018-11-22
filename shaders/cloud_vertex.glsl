#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 color;
layout (location = 3) in mat4 instanceOffset;

out vec3 FragPos;
out vec4 col;

uniform mat4 view;
uniform mat4 projection;

void main()
{   
    mat4 model = instanceOffset;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    FragPos = aPos;
    col = color;
}