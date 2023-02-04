#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexCoord;
layout (location=3) in uvec4 aJoints;
layout (location=4) in vec4 aWeights;

uniform mat4 u_jointMatrix[24];
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
    mat4 skinMatrix = u_jointMatrix[int(aJoints.x)] * aWeights.x
    + u_jointMatrix[int(aJoints.y)] * aWeights.y
    + u_jointMatrix[int(aJoints.z)] * aWeights.z
    + u_jointMatrix[int(aJoints.w)] * aWeights.w;

    mat4 world = u_model * skinMatrix;

    gl_Position = u_view * world * vec4(aPos.xyz, 1.0);
}