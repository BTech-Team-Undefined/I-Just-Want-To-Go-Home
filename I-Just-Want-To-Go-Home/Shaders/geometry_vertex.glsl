#version 450 core 

layout (location = 0) in vec3 i_Pos;	// position
layout (location = 1) in vec2 i_Uv;		// texture coordinates 
layout (location = 2) in vec3 i_Nrm;	// normal 
layout (location = 3) in vec3 i_Tan;	// tangent 
layout (location = 4) in vec3 i_Btn;	// bitangent

out vec3 f_Pos; 
out vec2 f_Uv; 
out vec3 f_Nrm; 

// world space frame of reference 
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() 
{
    f_Pos = (u_Model * vec4(i_Pos, 1.0)).xyz;
    f_Uv  = i_Uv;
    f_Nrm = transpose(inverse(mat3(u_Model))) * i_Nrm;    // decompose matrix to get rotation 
    gl_Position = u_Projection * u_View * u_Model * vec4(i_Pos, 1.0);
}