#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <stb\stb_image.h>
#include "Rendering\Material.h"
#include "Rendering\Mesh.h"
#include "Rendering\Renderable.h"
#include "Rendering\RenderComponent.h"
#include "Rendering\Constants.h"
#include "EntitySystems\Entity.h"
#include "EntitySystems\Component.h"

// Model loading adapted from tutorial https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h

class AssetLoader
{
public:
	AssetLoader() {};
	~AssetLoader() {};
	
	std::unique_ptr<Entity> LoadModel(const std::string& path)
	{
		// read file via assimp 
		Assimp::Importer importer; 
		const aiScene* scene = importer.ReadFile(path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		// check for errors 
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Failed to read file. Assimp Error: " << importer.GetErrorString() << std::endl;
			return nullptr;
		}

		// retrieve directory 
		std::string directory = path.substr(0, path.find_last_of('/'));
		this->directory = directory;

		// process root node recursively (gather all children)
		// TODO: FIX EVERYTHING
		auto rootEntity = std::make_unique<Entity>();
		// Entity* rootEntity = new Entity();
		ProcessNode(rootEntity.get(), scene->mRootNode, scene);

		// cleanup and return 
		this->directory = "";
		return rootEntity;
	}



private:
	// todo: vector of weak pointers 
	std::vector<TextureInfo> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
												// (this needs to be checked) currently this works on a per model basis (i.e textures shared between models or reloading a model will not be optimized)
												// uniform member is invalid here. make a copy if you wish to use the TextureInfo
	std::vector<TextureInfo> cubemaps_loaded;	// stores all the cubemap textures loaded so far. 
												// only id and path members are valid here. make a copy if you wish to use the TextureInfo.
	std::string directory;

	void ProcessNode(Entity* entity, aiNode* node, const aiScene* scene)
	{
		// check if there is anything to draw for this node 
		if (node->mNumMeshes > 0)
		{
			// add a render component 
			entity->addComponent<RenderComponent>();
			// process each mesh located at the current node & add renderable packages 
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene. 
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				auto r = Processmesh(mesh, scene);
				entity->getComponent<RenderComponent>()->addRenderable(r);
			}
		}

		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			Entity* newEntity = new Entity();
			newEntity->setLocalTransform(aiMatrix4x4ToGlm(node->mTransformation));
			entity->addChild(newEntity);
			ProcessNode(newEntity, node->mChildren[i], scene);
		}
	}

	std::shared_ptr<Renderable> Processmesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<TextureInfo> textures;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;	// whoa... 
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.Uv = vec;
			}
			else
			{
				vertex.Uv = glm::vec2(0.0f, 0.0f);
			}
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}

		// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			// TODO: concerned this will cause all models to be low poly visuals (loading indicies more than once)
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// create a renderable & material to house data in
		std::shared_ptr<Renderable> renderable = std::make_shared<Renderable>();
		renderable->mesh = std::make_shared<Mesh>(vertices, indices);
		renderable->material = std::make_shared<Material>();

		aiColor4D aiColor;
		// diffuse color 
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &aiColor))
			renderable->material->SetVec3(SHADER_DIFFUSE.c_str(), aiColor4DToVec3(aiColor));
		// specular color 
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &aiColor))
			renderable->material->SetVec3(SHADER_SPECULAR.c_str(), aiColor4DToVec3(aiColor));
		// ambient color 
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &aiColor))
			renderable->material->SetVec3(SHADER_AMBIENT.c_str(), aiColor4DToVec3(aiColor));
		// emissive color 
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &aiColor))
			renderable->material->SetVec3(SHADER_EMISSIVE.c_str(), aiColor4DToVec3(aiColor));
		// shininess strength 
		float shininess;
		if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess))
			renderable->material->SetFloat(SHADER_SHININESS.c_str(), shininess);

		// 1. diffuse maps
		std::vector<TextureInfo> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, SHADER_TEX_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<TextureInfo> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SHADER_TEX_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<TextureInfo> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, SHADER_TEX_NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<TextureInfo> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, SHADER_TEX_HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		// add texture to material 
		renderable->material->AddTextures(textures);
		
		// return a mesh object created from the extracted mesh data
		return renderable;
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<TextureInfo> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<TextureInfo> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				TextureInfo texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.uniform = typeName + std::to_string(i);
				texture.path = str.C_Str();
				// texture.type = typeName;
				
				// track 
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}

	
public:
	// load a texture.
	unsigned int LoadTexture(const std::string& path, int* width, int* height)
	{
		// check for a preloaded texture 
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), path.c_str()) == 0)
			{
				auto& t = textures_loaded[j];
				*width = t.width;
				*height = t.height;
				return t.id;
			}
		}

		// else load 
		auto id = TextureFromFile(path.c_str(), "", false, width, height);

		// track 
		TextureInfo texture;
		texture.id = id;
		texture.width = *width;
		texture.height = *height;
		texture.path = path;
		textures_loaded.push_back(texture);

		return id;
	}

	// load a texture into opengl, regardless if it's been loaded already. 
	unsigned int TextureFromFile(const char* path, const std::string& directory, 
		bool gamma = false, int* wOut = nullptr, int* hOut = nullptr)
	{
		std::cout << "Loading texture name: " << path << std::endl;

		std::string filename = (directory.empty()) ? std::string(path) : directory + '/' + std::string(path);

		// read image file 
		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		stbi_set_flip_vertically_on_load(true);
		
		if (data == NULL)
		{
			std::cout << "Failed to load texture: " << filename << std::endl;
			return -1;
		}
		
		// generate opengl texture 
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// determine format 
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		// load data 
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set parameters 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		if (wOut != nullptr) *wOut = width;
		if (hOut != nullptr) *hOut = height;
		return textureID;
	}

	// load a cubemap. will return a preloaded resource if found.
	unsigned int LoadCubemap(std::vector<std::string> facesPath)
	{
		std::string pathId = "";
		for (auto& s : facesPath)
			pathId += s;

		// check for a preloaded texture 
		for (unsigned int j = 0; j < cubemaps_loaded.size(); j++)
		{
			if (std::strcmp(cubemaps_loaded[j].path.data(), pathId.c_str()) == 0)
			{
				auto& t = cubemaps_loaded[j];
				return t.id;
			}
		}

		// else load 
		auto id = CubemapFromFiles(facesPath);
		
		// track 
		TextureInfo texture;
		texture.id = id;
		texture.path = pathId;
		cubemaps_loaded.push_back(texture);

		return id;
	}

	// https://learnopengl.com/Advanced-OpenGL/Cubemaps
	// load a cubemap texture into opengl with 6 specified paths to images. 
	// always loads, regardless if it's been loaded already.
	// order: right,left,top,bottom,front,back
	unsigned int CubemapFromFiles(std::vector<std::string> facesPath)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < facesPath.size(); i++)
		{
			unsigned char *data = stbi_load(facesPath[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				// determine format 
				GLenum format;
				if (nrChannels == 1)
					format = GL_RED;
				else if (nrChannels == 3)
					format = GL_RGB;
				else if (nrChannels == 4)
					format = GL_RGBA;

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cerr << "ERROR: Cubemap texture failed to load at path: " << facesPath[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	glm::vec3 aiColor4DToVec3(const aiColor4D &from)
	{
		return glm::vec3(from.r, from.g, from.b);
	}
	
	glm::vec3 aiVec3ToVec3(const aiVector3D &from)
	{
		return glm::vec3(from.x, from.y, from.x);
	}
};

