#version 420 core 

layout(location = 0) in vec4 i_Vertex; // <vec2 pos, vec2 tex>

out vec2 f_TexCoords;

uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * vec4(i_Vertex.xy, 0.0, 1.0);
	f_TexCoords = i_Vertex.zw;
}