#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 color;

uniform sampler2D u_texture;

void main()
{
    vec2 uv = texCoord;
    uv.y *= 900/1600.0;
    uv.y += 0.1;
    vec4 diffuse = texture(u_texture, uv);

    fragColor = vec4(diffuse.rgb * color, (1.0 - texCoord.y) / 1.3);
    fragColor = diffuse;
}