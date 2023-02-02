#version 330 core

uniform sampler2D u_texture_0;
uniform vec4 u_color;

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

void main()
{
    vec3 lightPos = vec3(5.0, 5.0, -5.0);
    vec3 viewDir = normalize(vec3(5.0, 5.0, 5.0) - fragPos);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 lightColor = vec3(1.0f, 0.9f, 0.6f);

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    vec4 color = texture(u_texture_0, texCoord) * u_color;

    float diff = max(dot(lightDir, normal.xyz), 0.0);
    vec3 diffuse = diff * color.rgb * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
    //vec3 specular = spec * lightColor * 0.5;

    vec3 ambient = color.rgb * 0.4;

    fragColor = vec4(ambient + diffuse * attenuation, color.a);
}