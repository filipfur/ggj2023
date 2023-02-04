#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform SceneBlock
{
    mat4 u_view;
    mat4 u_projection;
    vec3 u_camera_position;
    vec3 u_light_position;
    vec3 u_light_color;
    float u_time;
};

uniform mat4 u_model;

out vec3 fragPos;
out vec2 texCoords;

void main()
{
	fragPos = aPos;
    texCoords = aTexCoords;

    mat4 modelView = u_view * u_model;

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

	gl_Position = u_projection * modelView * vec4(aPos, 1.0f);
}