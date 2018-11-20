#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

const int plane_num = 11;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 models[plane_num];

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragPos;

void main()
{
    mat4 model = models[gl_InstanceID];
    gl_Position = projection * view * model * vec4(vertex, 1.0f);
  
    // Передает данные дальше в Fragment шейдер
    vNormal = mat3(transpose(inverse(model))) * normal;
    vFragPos = vec3(model * vec4(vertex, 1.0f));
    vTexCoords = texCoords;

}