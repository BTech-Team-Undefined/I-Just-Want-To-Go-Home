#version 450 core 

in vec2 f_Uv; 

out vec4 o_Col;

// I think order matters!? 
uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 

void main() 
{
    // retrieve data from gbuffer 
	vec3 pos = texture(u_PosTex, f_Uv).rgb;
	vec3 nrm = texture(u_NrmTex, f_Uv).rgb; 
	vec3 col = texture(u_ColTex, f_Uv).rgb; 

    // todo: no lighting yet :@) 

    o_Col = vec4(nrm, 1.0);
}