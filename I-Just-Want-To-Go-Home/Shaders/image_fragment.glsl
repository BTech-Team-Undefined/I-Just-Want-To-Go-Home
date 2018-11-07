#version 450 core

in vec2 f_Uv;

out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec4 u_Tint;

void main()
{
	o_Color = texture(u_Texture, f_Uv); // *u_Tint;
}