#version 330 core

in vec4 col;
in vec3 fragPos;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{   
    float color = col.x + 0.4;
    vec4 result = vec4(color,color,color, 0.8);
    if (color <= 0.3) {
        result.x += 0.5; result.y += 0.5; result.z += 0.5;
        result.w = 0.4;
    }
    fragColor = result;
    brightColor = vec4(0.0);
}