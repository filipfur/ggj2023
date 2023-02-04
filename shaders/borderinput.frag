#version 330 core

out vec4 fragColor;

uniform vec4 u_color;

float near = 1.0;
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    fragColor = vec4(u_color.rgb * pow((1.0 - depth), 3), 1.0);
}