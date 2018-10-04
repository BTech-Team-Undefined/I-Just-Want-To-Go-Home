#version 450 core 

in vec2 f_Uv; 
in vec4 f_LightPos;

out vec4 o_Col;

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

float near = 0.1;
float far  = 25.0; 


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
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;   // out of frustrum far plane

    return shadow;
}

// takes in non-normalized (raw) depth coordinates
float LinearizeDepth(float depth)
{
    // Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
    // Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() 
{
    // retrieve data from gbuffer 
	vec3 pos = texture(u_PosTex, f_Uv).rgb;
	vec3 nrm = texture(u_NrmTex, f_Uv).rgb; 
	vec4 col = vec4(texture(u_ColTex, f_Uv).rgb, 1.0); 
    float depth = texture(u_DphTex, f_Uv).x;
    
    // todo: no lighting yet :@) 

    if (u_DisplayPos)
    {
        float depths = LinearizeDepth(pos.z);
        o_Col = vec4(vec3(depths), 1.0);
        o_Col = vec4(pos, 1.0);
    }
    else if (u_DisplayNrm)
    {
        o_Col = vec4(nrm, 1.0);
    }
    else if (u_DisplayCol)
    {
        o_Col = col;
    }
    else if (u_DisplayDph)
    {
        o_Col = vec4(vec3(LinearizeDepth(depth) / far), 1.0);
    }
    else if (u_DisplayNoS)
    {
        // calculate ambient lighting 
        vec4 ambient = col * u_AmbientIntensity;
        vec3 surfToEye = normalize(u_ViewPosition - pos);
        // calculate point lights 
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            float lightDist = length(u_Lights[i].Position - pos);
            vec3 surfToLight = normalize(u_Lights[i].Position - pos);

            vec3 diffuse = max(dot(nrm, surfToLight), 0.0) * col.rgb * u_Lights[i].Color * 0.1;	// hardcoded 0.2 to reduce intensity
            float attenuation = 1.0 / (lightDist * ATTENUATION);
			attenuation *= attenuation;
            diffuse *= attenuation;
            ambient += vec4(diffuse, 1.0);
        }
        o_Col = ambient; 

        // // Another method? 
        // // calculate ambient lighting 
        // vec4 diffuse = col * u_AmbientIntensity;
        // vec3 surfaceToEye = pos - u_ViewPosition;
        
        // // calculate point lights 
        // for (int i = 0; i < MAX_LIGHTS; i++)
        // {
        //     // normal dot lightDir 
        //     vec3 lightDist = pos - u_Lights[i].Position;
        //     vec3 lightDir = normalize(lightDist);
        //     float intensity = max(dot(surfaceToEye, reflect(lightDir, nrm)), 0);
        //     float attenuation = 1.0 / (1.0 + ATTENUATION * pow(length(lightDist), 2));
        //     // white light? 
        //     diffuse = diffuse + col * intensity * attenuation;
        // }

        // o_Col = diffuse; 
    }
    else 
    {
	// vec3 pos = texture(u_PosTex, f_Uv).rgb;
	// vec3 nrm = texture(u_NrmTex, f_Uv).rgb; 
	// vec4 col = vec4(texture(u_ColTex, f_Uv).rgb, 1.0); 
    // float depth = texture(u_DphTex, f_Uv).x;
        vec3 lightColor = vec3(1.0);
        // ambient
        vec3 ambient = (0.10 * col).rgb;
        // diffuse 
        // vec3 lightDir = normalize(u_LightPos - pos); 
        float diff = 0.5; 
        // max(dot(lightDir, nrm), 0.0);
        vec3 diffuse = diff * lightColor;
        // shadow 
        float shadow = ShadowCalculation(vec4(pos, 1.0) * u_LightSpaceMatrix);
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse)) * col.rgb;   // not sure what this last * col is for. 
        o_Col = vec4(lighting, 1.0);
        
        // float yup = texture(u_ShadowMap, f_Uv).r;
        // o_Col = vec4(yup, yup, yup, 1.0);
        // o_Col = vec4(shadow, shadow, shadow, 1.0);

        // // o_Col = vec4(pos, 1.0);
        // vec3 huh = (vec4(pos, 1.0) * u_LightSpaceMatrix).xyz;
        // float hmm = huh.z;
        // // float hmm = (vec4(pos, 1.0) * u_LightSpaceMatrix).z;
        // o_Col = vec4(hmm, hmm, hmm, 1.0);
        // // o_Col = vec4(huh, 1.0);
    }
}