#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
    fragColor = vec4(0.4f, 0.4f, 0.8f, 1.0f);
    brightColor = vec4(0.0);
}
