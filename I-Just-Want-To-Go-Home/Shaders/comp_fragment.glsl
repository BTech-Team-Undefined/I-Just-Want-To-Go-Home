#version 420 core 

in vec2 f_Uv; 
in vec4 f_LightPos;

// Remember to set texture location via uniforms 
uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_ShadowMap;

// Debug control 
uniform bool u_DisplayPos; 
uniform bool u_DisplayNrm; 
uniform bool u_DisplayCol; 
uniform bool u_DisplayDph;
uniform bool u_DisplayNoS;

out layout(location = 0) vec4 o_Col;

// Lighting 
#define MAX_LIGHTS 32
#define ATTENUATION 0.05
struct Light 
{
    vec3 Position;
    vec3 Color;
};
uniform float u_AmbientIntensity = 1.0;
uniform Light u_Lights[MAX_LIGHTS];
uniform vec3 u_ViewPosition;
// Shadows 
uniform mat4 u_LightSpaceMatrix;    // todo: I need to do the shadow mapping calculations here (b/c i don't have a frag in vertex quad)
uniform vec3 u_LightPos;
uniform vec3 u_LightDir;

float near = 0.1;
float far  = 50.0; 
float shadowFadeThreshold = 0.95;
float minbias = 0.002;  // minimum shadow bias (for planes almost perpendicular to d.light) 
float maxbias = 0.002;   // maximum shadow bias (for planes with a steep slope to d.light)

// takes in non-normalized (raw) depth coordinates
float LinearizeDepth(float depth)
{
    // Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
    // Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

float ShadowCalculation(vec4 lightSpaceFrag)
{
    // perform perspective divide
    vec3 projCoords = lightSpaceFrag.xyz / lightSpaceFrag.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).x; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float bias = 0.002;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // handle steep angles 
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

	// soften shadows if near shadow map limit
	vec3 dist = abs(projCoords - vec3(0.5));
	float fade = max(max(dist.x, dist.y), dist.z) * 2.0;	// times 2 to bring range from 0-0.5 to 0-1.0
	if (fade > shadowFadeThreshold)
	{
		float percent = (fade - shadowFadeThreshold) / (1.0 - shadowFadeThreshold);
		shadow *= (1.0 - percent);
	}

	if (projCoords.z > 1.0)
		shadow = 0.0;   // out of frustrum far plane
		
    return shadow;
}

float ShadowCalcFade(vec4 lightSpaceFrag, float bias, float camDepth)
{
    // perform perspective divide
    vec3 projCoordsRaw = lightSpaceFrag.xyz / lightSpaceFrag.w;
    // transform to [0,1] range
    vec3 projCoords = projCoordsRaw * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).x; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // ----- hard -----
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // ----- soft -----
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    // ----- fade -----
    // length(projCoordsRaw)    == fade values close to very edge of the shadow map (including out)
    // abs(closest * 2.0 - 1.0) == fade values close to near/far plane (including out)
    float fade = max(length(projCoordsRaw), abs(closestDepth * 2.0 - 1.0));
    if (fade > shadowFadeThreshold)
	{
		float percent = (fade - shadowFadeThreshold) / (1.0 - shadowFadeThreshold);
		shadow *= (1.0 - clamp(percent, 0, 1));
	}

    return shadow;
}


float ShadowCalculationSoft(vec4 lightSpaceFrag)
{
    // perform perspective divide
    vec3 projCoords = lightSpaceFrag.xyz / lightSpaceFrag.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow + smooth out 
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;   // out of frustrum far plane

    return shadow;
}


void main() 
{
    // retrieve data from gbuffer 
	vec3 pos = texture(u_PosTex, f_Uv).rgb;
	vec3 nrm = texture(u_NrmTex, f_Uv).rgb; 
	vec4 col = vec4(texture(u_ColTex, f_Uv).rgb, 1.0); 
    float depth = texture(u_DphTex, f_Uv).x;

    // todo: no lighting yet :@) 
    if (depth >= 1.0)
    {
        discard;
    }

    // asdf

    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = (0.10 * col).rgb;
    // diffuse 
    // vec3 lightDir = normalize(u_LightPos - pos); 
    float diff = 0.5; 
    // max(dot(lightDir, nrm), 0.0);
    vec3 diffuse = diff * lightColor;

    // shadow 
    float bias = 0.002; 
    float shadow = ShadowCalcFade(u_LightSpaceMatrix * vec4(pos, 1.0), bias, LinearizeDepth(depth));
    // float shadow = ShadowCalculation(u_LightSpaceMatrix * vec4(pos, 1.0));
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse)) * col.rgb;   // not sure what this last * col is for. 
    o_Col = vec4(lighting, 1.0);
}