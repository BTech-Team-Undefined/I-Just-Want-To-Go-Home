#include "Mesh.h"
#include <gl\glad.h>

// { x y z | u v | nx ny nz }

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> idx)
{
	// tracking 
	this->vertices = verts; 
	this->indices = idx; 

	// initialize opengl data 
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_STATIC_DRAW);
	// position attribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
	glEnableVertexAttribArray(0);
	// uv attribute 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Uv)));
	glEnableVertexAttribArray(1);
	// normal attribute 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
	glEnableVertexAttribArray(2);
	// tangent attribute  
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Tangent)));
	glEnableVertexAttribArray(3);
	// bitangent attribute 
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Bitangent)));
	glEnableVertexAttribArray(4);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * idx.size(), &idx[0], GL_STATIC_DRAW);

	// finished - unbind
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	// this also deletes the buffers (in fact it's the only way to - http://alex-charlton.com/posts/When_is_it_okay_to_delete_an_OpenGL_buffer/)
	glDeleteVertexArrays(1, new unsigned int[1] { VAO });	
}

/* Notes:
	glEnableVertexAttribArray(0);	// this enables the variable in the shader 
	glVertexAttribPointer(
		0,					// shader variable ID/location
		3,					// element count
		GL_FLOAT,			// data type
		GL_FALSE,			// normalized
		8 * sizeof(float),	// size of data
		(void*)0			// stride
	);
	glEnableVertexAttribArray(0);	// you could do it here as well, doesn't matter!
*/