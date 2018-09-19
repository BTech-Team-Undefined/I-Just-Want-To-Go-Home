#version 450 core 

in vec2 f_Uv; 

out vec4 o_Col;

// I think order matters!? 
uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;

uniform bool u_DisplayPos; 
uniform bool u_DisplayNrm; 
uniform bool u_DisplayCol; 
uniform bool u_DisplayDph;
uniform float u_AmbientIntensity = 1.0;

float near = 0.1;
float far  = 25.0; 

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

    if (u_DisplayPos)
    {
        float depths = LinearizeDepth(pos.z);
        o_Col = vec4(vec3(depths), 1.0);
        o_Col = vec4(pos, 1.0);
    }
    else if (u_DisplayNrm)
    {
        o_Col = vec4(nrm, 1.0);
    }
    else if (u_DisplayCol)
    {
        o_Col = vec4(col, 1.0);
    }
    else if (u_DisplayDph)
    {
        o_Col = vec4(vec3(LinearizeDepth(depth) / far), 1.0);
    }
    else 
    {
        o_Col = vec4(col, 1.0) * u_AmbientIntensity;
    }
}