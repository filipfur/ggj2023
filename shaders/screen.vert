#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 FragPos;
out vec2 ShakeOffset;

uniform float u_time;
uniform float u_grayscale;
uniform vec2 u_shake_amplitude;
uniform vec2 u_shake_period;
uniform float u_shake_duration;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    FragPos = aPos.xy;
    vec2 shakePeriod = vec2(cos(u_time * u_shake_period.x), cos(u_time * u_shake_period.y));
    ShakeOffset = shakePeriod * u_shake_amplitude * u_shake_duration;
    TexCoords = aTexCoords;
}