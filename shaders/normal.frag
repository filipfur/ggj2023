#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;

void main()
{
    fragColor = vec4(normal.xyz, 1.0);
}