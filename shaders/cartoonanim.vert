#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(location = 3) in vec4 aBoneIds; 
layout(location = 4) in vec4 aWeights;

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

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        int boneId = int(aBoneIds[i]);
        if(boneId == -1) 
        {
            continue;
        }
        if(boneId >=MAX_BONES) 
        {
            totalPosition = vec4(aPos, 1.0f);
            totalNormal = aNormal;
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneId] * vec4(aPos,1.0f);
        totalPosition += localPosition * aWeights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneId]) * aNormal;
        totalNormal += localNormal * aWeights[i];
    }

    fragPos = vec3(u_model * vec4(totalPosition.rgb, 1.0));
    fragPosLightSpace = u_light_space * vec4(fragPos, 1.0);
    texCoord = aTexCoords;
    lightColor = u_light_color;
    mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    normal = normalize(normalMatrix * totalNormal.rgb);
    lightPos = u_light_position;
    viewPos = u_camera_position;

    gl_Position = u_projection * u_view * vec4(fragPos, 1.0);
}