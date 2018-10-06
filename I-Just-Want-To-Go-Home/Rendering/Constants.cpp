#include "Constants.h"

#include <string>

// Values from AI_MATKEY should be mapped into string variable names that .glsl files can use. 
// http://assimp.sourceforge.net/lib_html/materials.html
const std::string SHADER_DIFFUSE	   = "i_Diffuse";
const std::string SHADER_SPECULAR      = "i_Specular";
const std::string SHADER_AMBIENT	   = "i_Ambient";
const std::string SHADER_TEX_DIFFUSE   = "u_TexDiffuse";
const std::string SHADER_TEX_SPECULAR  = "u_TexSpecular";
const std::string SHADER_TEX_NORMAL    = "u_TexNormal";
const std::string SHADER_TEX_HEIGHT    = "u_TexHeight";
const std::string SHADER_LIGHT_D_POS   = "u_DirectionalLightPos";
const std::string SHADER_LIGHT_D_DIR   = "u_DirectionalLightDir";
const std::string SHADER_LIGHT_D_COLOR = "u_DirectionalLightColor";
// todo etc. 