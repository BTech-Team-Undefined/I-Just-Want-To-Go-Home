#version 420 core

in vec2 f_TexCoords;
out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec3 u_TextColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Texture, f_TexCoords).r);
	o_Color = vec4(u_TextColor, sampled.a) * sampled;
}