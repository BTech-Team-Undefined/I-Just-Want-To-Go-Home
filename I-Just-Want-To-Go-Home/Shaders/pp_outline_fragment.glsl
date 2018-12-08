// this is the template for post-processing fragment shader. 
// don't change the order and names of the variables! 

#version 420 core

in vec2 f_Uv;

uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_FinTex;

float threshhold = 0.00004;

float near = 0.1;
float far = 50.0;

out vec4 o_Color;

float LinearizeDepth(float depth)
{
	// Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
	// Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	vec3 origColor = texture(u_FinTex, f_Uv).rgb;
	vec2 tex_offset = 1.0 / textureSize(u_DphTex, 0);

	float unlinearizedDepth = texture(u_DphTex, f_Uv).r;
	float curDepth = unlinearizedDepth;
	float curLeftDepth = texture(u_DphTex, f_Uv - vec2(tex_offset.x, 0.0)).r;
	float curUpDepth = texture(u_DphTex, f_Uv - vec2(0.0, tex_offset.y)).r;
	float curRightDepth = texture(u_DphTex, f_Uv + vec2(tex_offset.x, 0.0)).r;
	float curDownDepth = texture(u_DphTex, f_Uv + vec2(0.0, tex_offset.y)).r;

	if (curLeftDepth - curDepth > threshhold
		|| curUpDepth - curDepth > threshhold
		|| curRightDepth - curDepth > threshhold
		|| curDownDepth - curDepth > threshhold) {
		o_Color = vec4(0.1, 0.1, 0.1, 1.0);
	}
	else {
		o_Color = vec4(origColor, 1.0);
	}
}