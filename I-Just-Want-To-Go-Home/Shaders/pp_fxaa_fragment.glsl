// https://www.youtube.com/watch?v=Z9bYzpwVINA
// Simple FXAA that uses 5 samples in an X pattern. 

#version 420 core

in vec2 f_Uv;

uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_FinTex;

out vec4 o_Color;

void main()
{
	if (f_Uv.x > 0.5)
	{
		float FXAA_SPAN_MAX = 8.0;
		float FXAA_REDUCE_MIN = 1.0/128.0;
		float FXAA_REDUCE_MAX = 1.0/8.0;

		vec2 tex_offset = 1.0 / textureSize(u_FinTex, 0);
		vec3 luma = vec3(0.299, 0.587, 0.114);	

		// sample 5 times, find luminosity 
		float lumaTR = dot(luma, texture2D(u_FinTex, f_Uv + vec2( 1.0,-1.0) * tex_offset).rgb);
		float lumaBL = dot(luma, texture2D(u_FinTex, f_Uv + vec2(-1.0, 1.0) * tex_offset).rgb);
		float lumaTL = dot(luma, texture2D(u_FinTex, f_Uv + vec2(-1.0,-1.0) * tex_offset).rgb);
		float lumaBR = dot(luma, texture2D(u_FinTex, f_Uv + vec2( 1.0, 1.0) * tex_offset).rgb);
		float lumaM  = dot(luma, texture2D(u_FinTex, f_Uv).rgb);

		// determine blur direction 
		vec2 dir;
		dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));	// top and bottom have large variance 
		dir.y = (lumaTL + lumaBL) - (lumaTR + lumaBR);		// left and right have large variance 

		// calculate magnitude of blur 
		float dirReduce = min((lumaTL + lumaTR + lumaBL + lumaBR) * (0.25 * FXAA_REDUCE_MAX), FXAA_REDUCE_MIN );
		float inverseDirAdjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
		
		dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), 
				max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * inverseDirAdjustment)) * tex_offset;

		vec3 result1 = (1.0/2.0) * (
			texture2D(u_FinTex, f_Uv.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
			texture2D(u_FinTex, f_Uv.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

		vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
			texture2D(u_FinTex, f_Uv.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
			texture2D(u_FinTex, f_Uv.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

		float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
		float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
		float lumaResult2 = dot(luma, result2);
		
		if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
			o_Color = vec4(result1, 1.0);
		else
			o_Color = vec4(result2, 1.0);
		}
	else 
	{
		o_Color = texture2D(u_FinTex, f_Uv);
	}
}

/*

Luminance (standard for certain colour spaces): (0.2126*R + 0.7152*G + 0.0722*B) [1]
Luminance (perceived option 1): (0.299*R + 0.587*G + 0.114*B) [2]
Luminance (perceived option 2, slower to calculate):  sqrt( 0.241*R^2 + 0.691*G^2 + 0.068*B^2 ) → sqrt( 0.299*R^2 + 0.587*G^2 + 0.114*B^2 )

*/