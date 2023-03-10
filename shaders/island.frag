#version 330 core

uniform sampler2D u_texture_0;
uniform sampler2D u_shadow_map_0;
uniform vec4 u_color;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec2 texCoord;
in vec3 normal;
in vec4 fragPosLightSpace;
in vec3 fragPos;
in vec3 viewPos;

float stylize(float f, float segments)
{
    return max(0.16, floor(f * segments + 0.3) / segments);
}

float ShadowCalculation(vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_shadow_map_0, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadow_map_0, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadow_map_0, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }
    return shadow;
}

const vec3 lightDir = vec3(0,1,0);

void main()
{
    vec4 color = texture(u_texture_0, texCoord * 1.0);
    color.a *= u_color.a;

    float shadow = ShadowCalculation(normal.xyz, lightDir) * 0.5;

    fragColor = vec4(color.rgb * stylize(1.0 - shadow, 2.0), color.a);

    vec3 avg = vec3((fragColor.r + fragColor.g + fragColor.b) / 3.0f);
    fragColor.rgb = mix(avg * vec3(0.8, 0.4, 0.1) * 0.4, fragColor.grb * vec3(0.75, 0.55, 0.1), u_color.r * smoothstep(0.7, 1.0, 1.0 - length((texCoord.xy - 0.5))));

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColor = vec4(fragColor.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}