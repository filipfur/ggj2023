#version 330 core

out vec4 fragColor;

uniform sampler2DMS diffuseTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D brightTexture;
uniform vec2 u_resolution;

mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

in vec2 TexCoords;

float near_plane = 1.0;
float far_plane = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

float SobelSampleDepth(in sampler2D t, in vec2 uv, in vec3 offset)
{
    float pixelCenter = LinearizeDepth(texture(t, uv).r);
    float pixelLeft   = LinearizeDepth(texture(t, uv - offset.xz).r);
    float pixelRight  = LinearizeDepth(texture(t, uv + offset.xz).r);
    float pixelUp     = LinearizeDepth(texture(t, uv + offset.zy).r);
    float pixelDown   = LinearizeDepth(texture(t, uv - offset.zy).r);

    return abs(pixelLeft - pixelCenter  +
           pixelRight - pixelCenter +
           pixelUp - pixelCenter    +
           pixelDown - pixelCenter);
}

vec4 SobelSample(in sampler2D t, in vec2 uv, in vec3 offset)
{
    vec4 pixelCenter = texture(t, uv);
    vec4 pixelLeft   = texture(t, uv - offset.xz);
    vec4 pixelRight  = texture(t, uv + offset.xz);
    vec4 pixelUp     = texture(t, uv + offset.zy);
    vec4 pixelDown   = texture(t, uv - offset.zy);

    return abs(pixelLeft - pixelCenter)  +
            abs(pixelRight - pixelCenter) +
            abs(pixelUp - pixelCenter)    +
            abs(pixelDown - pixelCenter);
}

const float outlineThickness = 1.4;
const float outlineDensity = 1.0;

const float outlineDepthMultiplier = 0.79;
const float outlineDepthBias = 0.35;

const float outlineNormalMultiplier = 0.8;
const float outlineNormalBias = 1.0;

const float minDepth  = 0.6;
const float depthSpan = 0.2;

void main()
{
    //vec2 uv = gl_FragCoord.xy / u_resolution;
    //uv -= 0.5;

    vec2 tex_offset = 1.0 / textureSize(depthTexture, 0) * outlineThickness;

    vec4 colorSample0 = texelFetch(diffuseTexture, ivec2(gl_FragCoord), 0);
    vec4 colorSample1 = texelFetch(diffuseTexture, ivec2(gl_FragCoord), 1);
    vec4 colorSample2 = texelFetch(diffuseTexture, ivec2(gl_FragCoord), 2);
    vec4 colorSample3 = texelFetch(diffuseTexture, ivec2(gl_FragCoord), 3);
    //float alpha = colorSample0.w * colorSample1.w * colorSample2.w * colorSample3.w;
    vec3 diffuse = vec3((colorSample0 + colorSample1 + colorSample2 + colorSample3) / 4.0f);

    vec3 bright = texture(brightTexture, TexCoords).rgb;

    //vec3 normal = texture(normalTexture, TexCoords).rgb;

    float depth01 = (LinearizeDepth(texture(depthTexture, TexCoords).r) / far_plane);

    float alpha = mix(1.0, 0.0, (clamp(depth01, minDepth, minDepth + depthSpan) - minDepth) / depthSpan);

    float sobelOutline = 0.0;
    float sobelDepth = 0.0;
    float sobelNormal = 0.0;
    if(alpha > 0)
    {
        sobelDepth = SobelSampleDepth(depthTexture, TexCoords, vec3(tex_offset, 0.0));
        sobelDepth = pow(abs(clamp(sobelDepth, 0.0, 1.0) * outlineDepthMultiplier), outlineDepthBias);

        vec3 sobelNormalVec = abs(SobelSample(normalTexture, TexCoords, vec3(tex_offset, 0.0)).rgb);
        sobelNormal = sobelNormalVec.x + sobelNormalVec.y + sobelNormalVec.z;
        sobelNormal = pow(abs(sobelNormal * outlineNormalMultiplier), outlineNormalBias);

        sobelOutline = clamp(max(sobelDepth, sobelNormal), 0.0, 1.0);
        sobelOutline = smoothstep(0.9, 1.0, sobelOutline) * alpha;
    }

    float exposure = 1.6;

    vec3 color = mix(diffuse + bright, vec3(0.0, 0.0, 0.0), sobelDepth * 1.0);
    // exposure tone mapping
    color = vec3(1.0) - exp(-color * exposure); // tone mapping
    // gamma correction 
    fragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    //fragColor = vec4(vec3(alpha), 1.0);
    //fragColor = vec4(vec3(depth01), 1.0);
    //fragColor = vec4(texture(normalTexture, TexCoords).rgb, 1.0);
    //fragColor = vec4(vec3(LinearizeDepth(texture(depthTexture, TexCoords).r) / far_plane), 1.0);
    //fragColor = vec4(bright, 1.0);
} 