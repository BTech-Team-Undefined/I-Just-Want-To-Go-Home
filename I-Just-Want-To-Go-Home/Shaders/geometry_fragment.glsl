#version 450 core 

in vec3 f_Pos; 
in vec2 f_Uv; 
in vec3 f_Nrm; 

out layout(location = 0) vec3 o_Pos; 
out layout(location = 1) vec3 o_Nrm; 
out layout(location = 2) vec4 o_Col; 

// opengl 4.2 you can bind this texture 
uniform sampler2D u_ColTex;
uniform sampler2D u_NrmTex;

void main() 
{
    o_Pos = f_Pos;
    o_Nrm = f_Nrm; 
    o_Col = texture(u_ColTex, f_Uv);
}