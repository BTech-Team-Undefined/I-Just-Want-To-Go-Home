#version 450 core

in vec3 f_TexCoords;

out vec4 FragColor;

uniform samplerCube u_TexCubemap;

void main()
{    
    FragColor = texture(u_TexCubemap, f_TexCoords);
}