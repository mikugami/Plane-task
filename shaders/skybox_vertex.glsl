#version 330 core

layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform mat4 view;

out vec3 vTexCoords;

void main()
{
    mat4 camera = mat4(mat3(view));
    vec4 pos = projection * camera * vec4(vertex, 1.0);
    gl_Position = pos;
    
    vTexCoords = vertex;
}  