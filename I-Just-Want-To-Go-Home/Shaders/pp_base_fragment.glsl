// this is the template for post-processing fragment shader. 
// don't change the order and names of the variables! 

#version 450 core

in vec2 f_Uv;

uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_FinTex;

out vec4 o_Color;

void main()
{
	o_Color = texture(u_FinTex, f_Uv);
}