#pragma once

/* Testing out visual studio type documentation */

/// <summary>
/// Customizable settings for a shader program. Each shader 
/// should have one material type. 
/// </summary>
class Material
{
public:
	Material();
	~Material();

	/// <summary>
	/// Loads material settings into opengl shader program
	/// </summary>
	virtual void LoadMaterial();

	/// <summary>
	/// Loads material settings into opengl shader program
	/// including any textures. 
	/// </summary>
	/// <param name="texStartLoc">Start address of the first free texture location.</param>
	/// <return>Number of textures bound.</return>
	virtual int LoadMaterial(unsigned int texStartLoc);
};

