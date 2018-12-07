#version 420 core

in vec2 f_Uv;

uniform sampler2D u_FinTex;

out vec4 o_Color;

void main()
{
	o_Color = texture(u_FinTex, f_Uv);
}