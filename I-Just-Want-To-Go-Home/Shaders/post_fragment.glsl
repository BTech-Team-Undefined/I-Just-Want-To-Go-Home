#version 420 core

in vec2 f_Uv;

out vec4 o_Color;

uniform sampler2D u_Pos;
uniform sampler2D u_Depth;
uniform vec4 u_Tint;
uniform vec3 u_ViewPosition;

float near = 0.1;
float far = 50.0;

float fog_start = 0.4;

vec3 fog_color = vec3(1.0, 1.0, 1.0);

float LinearizeDepth(float depth)
{
	// Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
	// Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

float CalcFog(float dist, float density) {
	//return dist;
	return 1 / exp((dist - fog_start) * density);
}

void main()
{
	float depth = texture(u_Depth, f_Uv).x;
	//vec3 relativePos = u_ViewPosition - pos;
	float fogStrength = 1 - CalcFog(LinearizeDepth(depth) / far, 0.98);
	o_Color = vec4(fog_color, fogStrength);
	//o_Color = vec4(col, 1.0);
}