// see post_fragment.glsl for original blended version 

#version 450 core

in vec2 f_Uv;

out vec4 o_Color;

uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_FinTex;
uniform float u_FogDensity = 2.0;
uniform float u_FogStart = 0.1;
uniform vec3 u_FogColor = vec3(1.0, 1.0, 1.0);

float near = 0.1;
float far = 50.0;

float LinearizeDepth(float depth)
{
	// Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
	// Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

float CalcFog(float dist, float density) 
{
	return 1 / exp((dist - u_FogStart) * density);
}

void main()
{
    vec3 color = texture(u_FinTex, f_Uv).rgb;
	float depth = texture(u_DphTex, f_Uv).x;
	float fogStrength = 1 - CalcFog(LinearizeDepth(depth) / far, u_FogDensity);
    // manual blending 
    color = (u_FogColor * fogStrength) + (color * (1-fogStrength));
    o_Color = vec4(color, 1.0);
}