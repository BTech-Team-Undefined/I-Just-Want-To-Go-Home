#include "Material.h"
#include <gl\glad.h>
#include <stb\stb_image.h>
#include <iostream>
#include "Constants.h"

Material::Material()
{
}

Material::Material(Shader* shader)
{
	// this->shader = shader;
}

Material::~Material()
{
}

void Material::LoadMaterial(const Shader* shader)
{
	// load all uniform data 
	for (const auto& elem : uniformsVec3)
	{
		shader->setVec3(elem.first, elem.second);
	}

	for (const auto& elem : uniformsFloat)
	{
		shader->setFloat(elem.first, elem.second);
	}

	for (const auto& elem : uniformsInt)
	{
		shader->setInt(elem.first, elem.second);
	}
}

int Material::LoadMaterial(const Shader* shader, unsigned int pos)
{
	// load uniform data
	LoadMaterial(shader);
	// load texture data 
	if (textures.size() == 0)
	{
		shader->setBool(SHADER_TEX_NONE, true);
	}
	else
	{
		shader->setBool(SHADER_TEX_NONE, false);
		for (int i = 0; i < textures.size(); i++)
		{
			// Set the uniform to point to texture 
			shader->setInt(textures[i].uniform, pos + i);
			// Activate the uniform variable 
			glActiveTexture(GL_TEXTURE0 + pos + i);
			// Bind the texture 
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			// std::cout << "Binding uniform to location: " << pos + i << " with name " << textures[i].uniform << std::endl;
		}
	}
	return textures.size();
}

// Creates a texture and stores the data 
bool Material::LoadTexture(const char* uniform, const char* texturePath)
{
	// https://learnopengl.com/Getting-started/Textures

	// read image file 
	int width, height, nrChannels;	// nrchannels 3 rgb, 4 rgba
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	stbi_set_flip_vertically_on_load(true);

	if (data == NULL)
	{
		std::cout << "Failed to load texture: " << texturePath << std::endl;
		return false;
	}

	// generate opengl texture 
	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	// set texture wrap/filter options (these are all default values)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load source image to opengl 
	glTexImage2D(
		GL_TEXTURE_2D,		// for 2D texture (not 1D or 3D)
		0,					// manual mipmap level. 0 for default.
		GL_RGB,				// data format. our albedo does not support alpha.
		width,				// width of texture 
		height,				// height of texture 
		0,					// legacy. always set to 0.
		GL_RGB,				// data format of source. jpg has no alpha.
		GL_UNSIGNED_BYTE,	// data format of source. stbi_load reads as bytes. 
		data				// data source. 
	);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// free source 
	stbi_image_free(data);

	// keep track 
	TextureInfo info; 
	info.uniform = uniform;
	info.id		 = texId;
	textures.push_back(info);

	return true;
}

void Material::AddTexture(TextureInfo info)
{
	textures.push_back(info);
}

void Material::AddTextures(std::vector<TextureInfo> infos)
{
	textures.insert(textures.end(), infos.begin(), infos.end());
}

void Material::SetVec3(const char * uniform, glm::vec3 value)
{
	uniformsVec3[uniform] = value;
}

void Material::SetFloat(const char * uniform, float value)
{
	uniformsFloat[uniform] = value;
}

void Material::SetInt(const char * uniform, int value)
{
	uniformsInt[uniform] = value;
}

void Material::SetBool(const char * uniform, bool value)
{
	uniformsInt[uniform] = value;
}

/* Notes:
	// load source image to opengl
	glTexImage2D(
		GL_TEXTURE_2D,		// for 2D texture (not 1D or 3D)
		0,					// manual mipmap level. 0 for default.
		GL_RGB,				// data format. our albedo does not support alpha.
		width,				// width of texture
		height,				// height of texture
		0,					// legacy. always set to 0.
		GL_RGB,				// data format of source. jpg has no alpha.
		GL_UNSIGNED_BYTE,	// data format of source. stbi_load reads as bytes.
		data				// data source.
	);
*/