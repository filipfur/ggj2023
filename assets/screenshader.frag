#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;

void main()
{
    fragColor = vec4(vec3(0.1), (1.0 - texCoord.y) / 1.3);
}