#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform SceneBlock
{
    mat4 u_view;
    mat4 u_projection;
    mat4 u_light_space;
    vec3 u_camera_position;
    vec3 u_light_position;
    vec3 u_light_color;
    float u_time;
};

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;
out vec3 lightColor;
out vec4 fragPosLightSpace;
out vec3 lightPos;
out vec3 viewPos;

uniform mat4 u_model;

void main()
{
    fragPos = vec3(u_model * vec4(aPos, 1.0));
    fragPosLightSpace = u_light_space * vec4(fragPos, 1.0);
    texCoord = aTexCoords;
    lightColor = u_light_color;
    mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    normal = normalize(normalMatrix * aNormal);
    lightPos = u_light_position;
    viewPos = u_camera_position;

    gl_Position = u_projection * u_view * vec4(fragPos, 1.0);
}