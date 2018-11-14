#include "Shader.h"
#include <string>
#include <fstream>	// file stream. used to handle reading/writing to files.
#include <sstream>	// string stream. used as a string buffer for streams. 
#include <iostream>	// std in/out. used to display messages to the user. 

#include <glm\glm.hpp>

Shader::Shader(const GLchar * vertexPath, const GLchar * fragPath)
{
	// 1. retrieve source file 
	std::string vertexCode; 
	std::string fragCode; 
	std::ifstream vFile; 
	std::ifstream fFile; 
	// ensure ifstream objects can throw exceptions 
	vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files 
		vFile.open(vertexPath);
		fFile.open(fragPath);
		std::stringstream vStream, fStream; 
		// read file buffer contents 
		vStream << vFile.rdbuf();
		fStream << fFile.rdbuf();
		// convert stream to string
		vertexCode	= vStream.str();
		fragCode	= fStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR: Failed to read contents to shader files." << std::endl;
	}
	// convert to constant string
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragCode.c_str();

	// 2. compile shaders 
	unsigned int vId;
	unsigned int fId;
	int success; 
	char infoLog[512];

	// vertex shader 
	vId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vId, 1, &vShaderCode, NULL);
	glCompileShader(vId);
	// display errors if any 
	glGetShaderiv(vId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vId, 512, NULL, infoLog);
		std::cout << "ERROR: Vertex shader failed to compile - " << vertexPath << std::endl << infoLog << std::endl;
	}

	// fragment shader 
	fId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fId, 1, &fShaderCode, NULL);
	glCompileShader(fId);
	// display errors if any 
	glGetShaderiv(fId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fId, 512, NULL, infoLog);
		std::cout << "ERROR: Fragment shader failed to compile. - " << fragPath << std::endl << infoLog << std::endl;
	}

	// 3. create shader program 
	programId = glCreateProgram();
	glAttachShader(programId, vId);
	glAttachShader(programId, fId);
	glLinkProgram(programId);
	// display linking errors if any 
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(programId, 512, NULL, infoLog);
		printf(infoLog);
		std::cout << "ERROR: Shader program failed to link for " << fragPath << " & " << vertexPath << ".\n" << infoLog << std::endl;
	}

	// 4. delete shaders as they're linked in the program already (wow TIL) 
	glDeleteShader(vId);
	glDeleteShader(fId);
}

Shader::~Shader()
{
}

void Shader::use()
{
	glUseProgram(programId);
}
