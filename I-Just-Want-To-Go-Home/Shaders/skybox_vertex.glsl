#version 420 core

layout(location = 0) in vec3 i_Pos;

out vec3 f_TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    f_TexCoords = i_Pos;
    gl_Position = u_Projection * u_View * vec4(i_Pos, 1.0);
}  