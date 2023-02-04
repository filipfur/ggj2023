#version 330 core

out vec4 FragColor;

uniform sampler2D u_texture_0;
uniform vec4 u_color;

in vec3 fragPos;
in vec2 texCoords;

void main()
{
	vec4 diffuse = texture(u_texture_0, texCoords.xy).rgba;
	FragColor = vec4(0.0, 0.0, 0.0, diffuse.a);
	//FragColor = vec4(u_color.rgb * 2.0, clamp(1.0 - length(fragPos), 0.0, 1.0));
}