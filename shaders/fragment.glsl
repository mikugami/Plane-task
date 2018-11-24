#version 330 core

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

uniform samplerCube skybox_texture;
uniform sampler2D plane_texture;

uniform vec3 color_light;
uniform vec3 light_pos;
uniform vec3 view_pos;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
    // Расчет освещения
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

    vec3 light_result = (ambient + diffuse + specular);

    // Отражение в стекле самолета
    vec4 glass_color = vec4(37.0/255.0, 47.0/255.0, 43.0/255.0, 1.0);
    vec3 plane_color = texture(plane_texture, vTexCoords).xyz;

    vec3 result = plane_color * light_result;

    if (plane_color.x <= glass_color.x + 0.01 &&  plane_color.x >= glass_color.x - 0.01 
            && plane_color.y <= glass_color.y + 0.01 && plane_color.y >= glass_color.y - 0.01
            && plane_color.z <= glass_color.z + 0.01 && plane_color.z >= glass_color.z - 0.01) {
        vec3 I = normalize(vFragPos - view_pos);
        vec3 R = reflect(I, normalize(vNormal));

        vec3 skybox_color = texture(skybox_texture, R).xyz;

        result = skybox_color * light_result;
    }

    // Расчёт карты бликов
    //float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    float brightness = dot(result, vec3(0.2526, 0.7452, 0.0822));
    if (brightness > 1.0) {
        brightColor = vec4(result, 1.0);
    } else {
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    fragColor = vec4(result, 1.0);
}