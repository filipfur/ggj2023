#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstance;

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
out vec4 fragPosLightSpace;
//out vec3 lightPos;
out vec3 viewPos;

out float instanceId;

uniform mat4 u_model;

void main()
{
    /*mat4 modelView = aInstance * u_view;
    // Billboard FTW!
    modelView[0][0] = 1.0;
    modelView[0][1] = 0.0;
    modelView[0][2] = 0.0;

    modelView[1][0] = 0.0;
    modelView[1][1] = 1.0;
    modelView[1][2] = 0.0;

    modelView[2][0] = 0.0;
    modelView[2][1] = 0.0;
    modelView[2][2] = 1.0;

    gl_Position = u_projection * modelView * vec4(aPos, 1.0);*/

    fragPos = vec3(aInstance * vec4(aPos, 1.0));
    fragPosLightSpace = u_light_space * vec4(fragPos, 1.0);
    texCoord = aTexCoords;
    mat3 normalMatrix = transpose(inverse(mat3(aInstance)));
    normal = normalize(normalMatrix * aNormal);
    //lightPos = u_light_position;

    instanceId = float(gl_InstanceID);

    viewPos = u_camera_position;

    gl_Position = u_projection * u_view * vec4(fragPos, 1.0);
}