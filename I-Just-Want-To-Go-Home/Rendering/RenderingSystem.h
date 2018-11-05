#pragma once

#include <vector>
#include <ft2build.h>
#include <freetype\freetype.h>
#include "OpenGLProfiler.h"
#include "Renderable.h"
#include "RenderComponent.h"
#include "../Camera.h"
#include "Lighting\Light.h"
#include "Lighting\DirectionalLight.h"
#include "../EntitySystems/System.h"


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


class RenderingSystem : public System
{
public:
	Camera* activeCamera; 
	Shader* geometryShader;		// default geometry shader 
	Shader* compositionShader;	// default composition shader 
	Shader* shadowmapShader;	// default shadowmap shader 
	Shader* textShader;			// default text shader 

private:
	OpenGLProfiler profiler;
	unsigned int quadVAO;
	unsigned int FBO, posTex, nrmTex, colTex, dphTex;
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	unsigned int screenWidth, screenHeight; 

	//std::map<GLchar, Character> Characters;
	FT_Library ft;
	glm::mat4 textProjection;
	unsigned int textVAO;	// quad to draw a single letter 
	unsigned int textVBO;	// quad buffer to draw a single letter
	std::map <std::string, std::map<GLchar, Character>> _fonts;
	
	std::vector<RenderComponent*> _components;
	std::vector<DirectionalLight *> _dlights;
	std::vector<Camera*> _cameras;

public:
	RenderingSystem();
	~RenderingSystem();
	void SetSize(unsigned int width, unsigned int height);
	void SetCamera(Camera* camera);
	virtual void update(float dt) override;
	virtual void addComponent(std::type_index t, Component* component) override;
	virtual void clearComponents() override;
	void LoadFont(std::string path);

private: 
	void RenderGeometryPass();
	void RenderEntityGeometry(Entity* e, glm::mat4 transform);
	void RenderCompositionPass();
	void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, std::string font = RENDERING_SYSTEM_DEFAULT_FONT);
	void InitializeFrameBuffers();
	void InitializeScreenQuad();
	void InitializeTextEngine();
	void DrawComponent(RenderComponent* component);
};

