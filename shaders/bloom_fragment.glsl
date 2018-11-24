#version 330 core

in vec2 vTexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;

out vec4 fragColor;

void main()
{             
    vec3 sceneColor = texture(scene, vTexCoords).xyz;          
    vec3 bloomColor = texture(bloomBlur, vTexCoords).xyz;
    if(bloom)
        sceneColor += bloomColor;
    fragColor = vec4(sceneColor, 1.0);
}