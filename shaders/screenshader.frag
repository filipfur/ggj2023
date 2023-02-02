#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 color;

uniform sampler2D u_texture;

void main()
{
    vec4 diffuse = texture(u_texture, texCoord);

    fragColor = vec4(diffuse.rgb * color, (1.0 - texCoord.y) / 1.3);
}