#version 330 core

uniform vec4 u_color;

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;

void main()
{
    fragColor = u_color;
}