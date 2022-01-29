///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_SAT_DEMO_BUILD
#ifdef WITH_SAT_DEMO_MODEL_FILES

#include <iostream>
#include "SATLoadedMesh.h"
#include <stdexcept> //for stuff not implemented in gles2, if we throw no errors after fix up, remove this!

#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"

namespace SAT
{
	void LoadedMesh::setupMesh()
	{
		/*glGenVertexArrays(1, &VAO);*/
		//glBindVertexArray(VAO);
		ec(glGenBuffers(1, &VBO));
		ec(glGenBuffers(1, &EAO));
		ec(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		ec(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW));
		ec(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAO));
		ec(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW));

		enableAttributes();
	}

	void LoadedMesh::enableAttributes()
	{
		//enable vertex data) 
		ec(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0)));
		ec(glEnableVertexAttribArray(0));

		//enable normal dat)a
		ec(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal))));
		ec(glEnableVertexAttribArray(1));

		//enable texture coordinate data
		ec(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, textureCoords))));
		ec(glEnableVertexAttribArray(2));
	}

	void LoadedMesh::disableAttributes()
	{
		ec(glDisableVertexAttribArray(0));
		ec(glDisableVertexAttribArray(1));
		ec(glDisableVertexAttribArray(2));
	}

	LoadedMesh::LoadedMesh(std::vector<Vertex> vertices, std::vector<MaterialTexture> textures, std::vector<unsigned int> indices) : vertices(vertices), textures(textures), indices(indices)
	{
		setupMesh();
	}

	LoadedMesh::~LoadedMesh()
	{
		//be careful, copy ctor kills buffers silently if these are uncommented (currently happening in instancing tutorial)
		//glDeleteBuffers(1, &VBO);
		//glDeleteBuffers(1, &EAO);
		//glDeleteBuffers(1, &modelVBO);
		//glDeleteVertexArrays(1, &VAO);
	}

	/** This really should be a private function, making visible for instancing tutorial */
	//GLuint LoadedMesh::getVAO()
	//{
	//	return VAO;
	//}

	void LoadedMesh::setInstancedModelMatrixVBO(GLuint modelVBO)
	{
		this->modelVBO = modelVBO;
	}

	void LoadedMesh::setInstancedModelMatricesData(glm::mat4* modelMatrices, unsigned int count)
	{
		//glBindVertexArray(VAO);
		enableAttributes();

		if (modelVBO == 0)
		{
			throw std::runtime_error("not implemented in gles2, not all calls can be translated to gles2 easily, so will have to fix this up if ever used.");
			//glGenBuffers(1, &modelVBO);
			//glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * count, &modelMatrices[0], GL_STATIC_DRAW);
			//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(0));
			//glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(sizeof(glm::vec4)));
			//glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(2 * sizeof(glm::vec4)));
			//glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(3 * sizeof(glm::vec4)));
			//glEnableVertexAttribArray(3);
			//glEnableVertexAttribArray(4);
			//glEnableVertexAttribArray(5);
			//glEnableVertexAttribArray(6);
			//glVertexAttribDivisor(3, 1);
			//glVertexAttribDivisor(4, 1);
			//glVertexAttribDivisor(5, 1);
			//glVertexAttribDivisor(6, 1);
		}
		else
		{
			std::cerr << "instanced model matrices VBO already exists" << std::endl;
		}

		//glBindVertexArray(0);
		disableAttributes();
	}

}
#endif //WITH_SAT_DEMO_MODEL_FILES
#endif //WITH_SAT_DEMO_BUILD
