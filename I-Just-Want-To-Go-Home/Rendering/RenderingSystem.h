#pragma once

#include <vector>
#include <ft2build.h>
#include <freetype\freetype.h>
#include "OpenGLProfiler.h"
#include "../Core/CpuProfiler.h"
#include "Renderable.h"
#include "RenderComponent.h"
#include "../Camera.h"
#include "Lighting\Light.h"
#include "Lighting\DirectionalLight.h"
#include "Lighting\PointLight.h"
#include "../EntitySystems/System.h"
#include "UI\View.h"
#include "UI\ImageComponent.h"
#include "UI\TextComponent.h"
#include "PostProcess\PostProcess.h"


#define RENDERING_SYSTEM_DEFAULT_FONT "fonts/arial.ttf"
#define RENDERING_SYSTEM_DEFAULT_FONTSIZE 48
#define FREETYPE_DYNAMIC_WIDTH 0	

// A freetype character 
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

// Font information used to render 
struct FontInfo
{
	std::string Path;
	GLuint LineHeight;
	std::map<GLchar, Character> Characters;
};

class RenderingSystem : public System
{
public:
	Camera* activeCamera; 
	Shader* geometryShader;		// default geometry shader 
	Shader* compDLightShader;	// default composition shader for directional lights
	Shader* compPLightShader;	// default composition shader for point lights
	Shader* shadowmapShader;	// default shadowmap shader 
	Shader* textShader;			// default text shader 
	Shader* imageShader;		// default UI shader 
	Shader* postShader;			// default postprocessing shader 
	Shader* postToScreenShader;	// default shader to move final texture to back buffer
	Shader* skyboxShader;		// default shader for skybox

	unsigned int skyboxTexture;

private:
	OpenGLProfiler profiler;
	CpuProfiler cpuProfiler;
	unsigned int quadVAO, cubeVAO;
	unsigned int FBO, posTex, nrmTex, colTex, dphTex, finWriteTex, finReadTex;
	unsigned int ppWriteFBO, ppReadFBO;
	unsigned int screenWidth, screenHeight; 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	unsigned int ppAttachments[1] = { GL_COLOR_ATTACHMENT0 };

	FT_Library ft;
	unsigned int textVAO;	// quad to draw a single letter 
	unsigned int textVBO;	// quad buffer to draw a single letter
	glm::mat4 uiProjection;

	std::map<std::string, FontInfo> fonts;
	std::vector<RenderComponent*> _components;
	std::vector<DirectionalLight *> _dlights;
	std::vector<PointLight *> _plights;
	std::vector<Camera*> _cameras;
	std::vector<TextComponent*> _texts;
	std::vector<ImageComponent*> _images; 

	std::map<std::string, std::unique_ptr<PostProcess>> _postProcesses;

	glm::mat4 projection;
	glm::mat4 view;


public:
	RenderingSystem();
	~RenderingSystem();
	void SetSize(unsigned int width, unsigned int height);
	void SetCamera(Camera* camera);
	virtual void update(float dt) override;
	virtual void addComponent(std::type_index t, Component* component) override;
	virtual void clearComponents() override;
	void LoadFont(std::string path);
	void addPostProcess(const std::string name, std::unique_ptr<PostProcess> postProcess);
	void removePostProcess(const std::string name);
	PostProcess* getPostProcess(const std::string name);
	void setSkybox(unsigned int cubemapId);

private: 
	
	void RenderGeometryPass();

	void RenderDirectionalLightingPass();	// composition
		
	void RenderPointLightingPass();			// composition 

	void RenderShadowMapsPass();

	void RenderSkyboxPass();

	void RenderPostProcessPass();

	void RenderUIImagesPass();

	void RenderUITextPass();

	void RenderEntityGeometry(Entity* e, glm::mat4 transform);
	void RenderCompositionPass();
	void RenderText(Shader &s, std::string text, TextAlignment alignment, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, std::string font = RENDERING_SYSTEM_DEFAULT_FONT);
	void RenderImage(Shader &s, ImageComponent* image);
	void InitializeFrameBuffers();
	void InitializeScreenQuad();
	void InitializeScreenCube();
	void InitializeTextEngine();
	void DrawComponent(RenderComponent* component);
};

