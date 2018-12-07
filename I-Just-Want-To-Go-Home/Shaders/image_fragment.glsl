#version 420 core

in vec2 f_Uv;

out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec3 u_Tint;
uniform float u_Opacity;

void main()
{
	vec4 color = texture(u_Texture, f_Uv) * vec4(u_Tint, 1.0);
	color.a = color.a * u_Opacity;
	o_Color = color;
}