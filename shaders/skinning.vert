#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexCoord;
layout (location=3) in uvec4 aJoints;
layout (location=4) in vec4 aWeights;

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

uniform mat4 u_jointMatrix[24];

out vec3 fragPos;
out vec3 normal;
out vec4 weights;
out vec4 joints;
out vec2 texCoord;
out vec3 lightColor;
out vec4 fragPosLightSpace;
out vec3 lightPos;
out vec3 viewPos;

uniform mat4 u_model;

void main()
{
    fragPos = vec3(u_model * vec4(aPos.xyz, 1.0));
    fragPosLightSpace = u_light_space * vec4(fragPos, 1.0);
    texCoord = aTexCoord;
    //texCoord.y = 1.0 - texCoord.y;
    lightColor = u_light_color;
    //mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    //normal = normalize(normalMatrix * totalNormal.rgb);
    normal = mat3(u_model) * aNormal;
    lightPos = u_light_position;
    viewPos = u_camera_position;

    mat4 skinMatrix = u_jointMatrix[int(aJoints.x)] * aWeights.x
    + u_jointMatrix[int(aJoints.y)] * aWeights.y
    + u_jointMatrix[int(aJoints.z)] * aWeights.z
    + u_jointMatrix[int(aJoints.w)] * aWeights.w;

    joints = vec4(aJoints);
    weights = aWeights;

    mat4 world = u_model * skinMatrix;

    gl_Position = u_projection * u_view * world * vec4(aPos.xyz, 1.0);
}