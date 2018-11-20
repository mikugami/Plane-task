#version 330 core

///*
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
//*/

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

uniform sampler2D color_texture;
uniform vec3 color_light;
uniform vec3 light_pos;
uniform vec3 view_pos;

/*
out vec4 color;
*/

void main()
{

    float ambientStrenght = 0.4f;
    vec3 ambient = ambientStrenght * color_light;

    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(light_pos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color_light;

    float specularStrength = 3.0f;
    vec3 viewDir = normalize(view_pos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * color_light;

    /*
    vec3 light_result = (ambient + diffuse + specular);
    vec4 light_color = vec4(light_result, 1.0f);

    color = texture(color_texture, vTexCoords) * light_color;
    */

    ///*
    vec3 light_result = (ambient + diffuse + specular);
    vec3 result = texture(color_texture, vTexCoords).rgb * light_result;

    //float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    float brightness = dot(result, vec3(0.2526, 0.7452, 0.0822));
    if (brightness > 1.0)
        brightColor = vec4(result, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);

    fragColor = vec4(result, 1.0);
    //*/
}