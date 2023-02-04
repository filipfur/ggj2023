#version 330 core
out vec4 fragColor;
  
uniform sampler2D diffuseTexture;
  
uniform vec2 u_resolution;

uniform int u_horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

/*vec3 multiSample(in sampler2DMS texSampler, in ivec2 uv)
{
    vec3 sample0 = texelFetch(texSampler, ivec2(uv), 0).rgb;
    vec3 sample1 = texelFetch(texSampler, ivec2(uv), 1).rgb;
    vec3 sample2 = texelFetch(texSampler, ivec2(uv), 2).rgb;
    vec3 sample3 = texelFetch(texSampler, ivec2(uv), 3).rgb;
    return (sample0 + sample1 + sample2 + sample3) / 4.0;
}*/

in vec2 TexCoords;

void main()
{             
    //vec2 tex_offset = 1.0 / u_resolution; // gets size of single texel
    vec2 tex_offset = 1.0 / textureSize(diffuseTexture, 0);

    //vec2 TexCoords = gl_FragCoord.xy / u_resolution;

    vec3 result = texture(diffuseTexture, TexCoords).rgb * weight[0];

    //vec3 result = multiSample(diffuseTexture, ivec2(gl_FragCoord)) * weight[0]; // current fragment's contribution

    if(u_horizontal == 1)
    {
        /*for(int i = -4; i < 5; ++i)
        {
            result += multiSample(diffuseTexture, ivec2(gl_FragCoord.x + tex_offset.x * i, gl_FragCoord.y)) * weight[i];
        }*/
        for(int i = 1; i < 5; ++i)
        {
            result += texture(diffuseTexture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(diffuseTexture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        /*for(int i = -4; i < 5; ++i)
        {
            result += multiSample(diffuseTexture, ivec2(gl_FragCoord.x, gl_FragCoord.y + tex_offset.y * i)) * weight[i];
        }*/
        for(int i = 1; i < 5; ++i)
        {
            result += texture(diffuseTexture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(diffuseTexture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    fragColor = vec4(result, 1.0);
    //fragColor = vec4(texture(diffuseTexture, TexCoords).rgb, 1.0);
    //fragColor = vec4(tex_offset * gl_FragCoord.xy * 100.0, 0, 1.0);
}