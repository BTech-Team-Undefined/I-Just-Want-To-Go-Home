#pragma once

#include <vector>
#include <utility>
#include <string>
#include <map>

#include <assimp\material.h>

#include "Shader.h"

/* Testing out visual studio type documentation */



struct TextureInfo
{
	std::string uniform;
	unsigned int id; 
	std::string path;	// prevent double loading of textures 
};

/// <summary>
/// Customizable settings for a shader program. Each shader 
/// should have one material type. 
/// </summary>
class Material
{
// variables 
protected: 
	/// Shader that this material is for 
	// Shader* shader;						

	/// Textures that this material has 
	std::vector<TextureInfo> textures; 
	// std::pair<std::string, unsigned int> textures; 

	/// Stores uniform values in the form of vector 3.
	/// The key is the variable name in shader.
	std::map<std::string, glm::vec3> uniformsVec3;

	/// Stores uniform values in the form floats.
	/// The key is the variable name in shader.
	std::map<std::string, float> uniformsFloat;

	/// Stores uniform values in the form of integers (and booleans).
	/// The key is the variable name in shader.
	std::map<std::string, int> uniformsInt;

// functions 
public:
	Material();
	Material(Shader* shader);
	~Material();

	/// <summary>
	/// Loads material settings into opengl shader program
	/// </summary>
	virtual void LoadMaterial(const Shader* shader);

	/// <summary>
	/// Loads material settings into opengl shader program
	/// including any textures. 
	/// </summary>
	/// <param name="texStartLoc">Start address of the first free texture location.</param>
	/// <return>Number of textures bound.</return>
	virtual int LoadMaterial(const Shader* shader, unsigned int texStartLoc);

	/// <summary>
	/// Loads a texture from path.
	/// </summary>
	/// <return>True if success, false if failed.</return>
	bool LoadTexture(const char* uniform, const char* path);


	void AddTexture(TextureInfo info);

	void AddTextures(std::vector<TextureInfo> infos);


	void SetVec3(const char* uniform, glm::vec3 value);

	void SetFloat(const char* uniform, float value);

	void SetInt(const char* uniform, int value);

	void SetBool(const char* uniform, bool value);
};

