#version 330 core

uniform sampler2D u_texture_0;
uniform sampler2D u_shadow_map_0;
uniform vec4 u_color;
uniform float iTime;

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

void main()
{

    vec2 uv = texCoord;
    uv.x += iTime * 0.0005;
    uv.y += sin(uv.x + iTime * 0.5) * 0.0005;

    vec4 color = texture(u_texture_0, uv * 24.0) * u_color;

    vec3 lightDir = normalize(vec3(0.1,1,0.1));

    float diff = max(dot(lightDir, normal.xyz), 0.0);

    float shadow = ShadowCalculation(normal.xyz, lightDir) * 0.5;

    //vec3 viewDir = normalize(viewPos - fragPos);
    //float fresnel = dot(normal.xyz, viewDir);

    float rim = 1.0 - max(dot(viewPos, normal.xyz), 0.0);
    rim = smoothstep(0.0, 1.0, rim);
    rim = 0.0;

    //fragColor = vec4(color.rgb * stylize(diff, 2.0) * (1.0 - shadow) + color.rgb * vec3(rim) * 0.5, color.a);
    fragColor = vec4(color.rgb * stylize(diff * (1.0 - shadow), 2.0) + color.rgb * stylize(rim * 0.5, 2.0), min(fragPos.y / 1.0, 0.4)) * vec4(2.0, 2.0, 3.5, 1.0);
    //fragColor = vec4(vec3(rim), 1.0);
    //fragColor = vec4(normal.xyz, 1.0);
    //fragColor = vec4(vec3(shadow), 1.0);
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColor = vec4(fragColor.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}