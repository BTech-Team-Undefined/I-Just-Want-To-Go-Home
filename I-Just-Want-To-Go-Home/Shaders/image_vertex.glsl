#version 420 core 

layout(location = 0) in vec3 i_Pos; 
layout(location = 1) in vec2 i_Uv;

out vec2 f_Uv;

uniform mat4 u_Projection;
uniform mat4 u_Model;
uniform vec2 u_Size; 

void main()
{
	f_Uv = i_Uv;
	// gl_Position = u_Projection * vec4(i_Pos, 1.0) * vec4(u_Size, 1.0, 1.0); // not working? 
	gl_Position = u_Projection * u_Model * vec4(i_Pos.x * u_Size.x, i_Pos.y * u_Size.y, i_Pos.z, 1.0);	// flip 
}