#version 420 core

in vec2 f_Uv;

uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_FinTex;

out vec4 o_Color;

void main()
{
	vec4 col = texture(u_FinTex, f_Uv);
	o_Color = vec4(1 - col.r, 1 - col.g, 1 - col.b, 1);
}