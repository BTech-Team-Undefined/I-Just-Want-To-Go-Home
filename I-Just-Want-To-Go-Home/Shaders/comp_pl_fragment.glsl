#version 450 core 

in vec2 f_Uv; 
in vec4 f_LightPos;

// Remember to set texture location via uniforms 
uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_ShadowMap;

out layout(location = 0) vec4 o_Col;

uniform vec3 u_ViewPosition;

// Lighting 
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform float u_LightIntensity;
uniform float u_LightRange;

float atten_linear = 0.1; 
float atten_quadratic = 0.1; 
float near = 0.1;
float far  = 50.0; 

// takes in non-normalized (raw) depth coordinates
float LinearizeDepth(float depth)
{
    // Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
    // Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() 
{
    // retrieve data from gbuffer 
	vec3 pos = texture(u_PosTex, f_Uv).rgb;
	vec3 nrm = texture(u_NrmTex, f_Uv).rgb; 
	vec3 col = texture(u_ColTex, f_Uv).rgb; 
    float depth = texture(u_DphTex, f_Uv).x;

    // todo: no lighting yet :@) 
    if (depth >= 1.0)
    {
        discard;
    }

    float distance = length(u_LightPos - pos);

    if (distance > u_LightRange)
    {
        o_Col = vec4(vec3(0), 1.0);
        return;
    }

    /* exponential  
    float attenutation = 1.0 / (1.0  
        + atten_linear * distance  
        + atten_quadratic * (distance * distance));
    */

    // upside down parabola 
    float attenutation = ((distance * distance) / -(u_LightRange * u_LightRange)) + 1.0;

    // // right way up parabola (softer, better)
    // float attenutation = (1 / (u_LightRange * u_LightRange)) * (distance * distance - 2 * u_LightRange * distance + u_LightRange * u_LightRange);

    vec3 diffuse = u_LightColor * attenutation; // *col;
    o_Col = vec4(diffuse, 1.0);
}