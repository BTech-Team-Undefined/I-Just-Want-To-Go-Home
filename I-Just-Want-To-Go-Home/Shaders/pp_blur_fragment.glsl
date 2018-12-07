// this is the template for post-processing fragment shader. 
// don't change the order and names of the variables! 

#version 450 core

in vec2 f_Uv;

uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_FinTex;

out vec4 o_Color;

// might want to split this into two shaders 
#define SAMPLE_SIZE 5
uniform bool u_Horizontal;
float weight[SAMPLE_SIZE] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

// https://learnopengl.com/Advanced-Lighting/Bloom
void main()
{
	vec2 tex_offset = 1.0 / textureSize(u_FinTex, 0);
	vec3 result = texture(u_FinTex, f_Uv).rgb * weight[0];

   	if(u_Horizontal)
    {
        for(int i = 1; i < SAMPLE_SIZE; ++i)
        {
            result += texture(u_FinTex, f_Uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(u_FinTex, f_Uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < SAMPLE_SIZE; ++i)
        {
            result += texture(u_FinTex, f_Uv + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(u_FinTex, f_Uv - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

	o_Color = vec4(result, 1.0);
}