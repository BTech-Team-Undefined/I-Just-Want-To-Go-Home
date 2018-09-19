#version 450 core 

in vec2 f_Uv; 

out vec4 o_Col;

// Remember to set texture location via uniforms 
uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;

// Debug control 
uniform bool u_DisplayPos; 
uniform bool u_DisplayNrm; 
uniform bool u_DisplayCol; 
uniform bool u_DisplayDph;

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

float near = 0.1;
float far  = 25.0; 

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
    else 
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
}