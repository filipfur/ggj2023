#version 330 core

uniform sampler2D u_texture_0;
uniform vec4 u_color;

layout (location = 0) out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;


void main()
{
    vec4 color = texture(u_texture_0, texCoord) * u_color;

    float brightness = dot(color.rgb, vec3(1.0, 1.0, 0.5742));
    if(color.r * color.g * color.b > 0 && brightness > 1.5)
        fragColor = vec4(color.rgb * 2.0, 1.0);
    else
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}