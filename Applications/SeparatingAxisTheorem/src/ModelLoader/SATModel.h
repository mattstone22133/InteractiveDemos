#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_SAT_DEMO_BUILD
#include <vector>
#include "SATLoadedMesh.h"

#ifdef WITH_ASSIMP
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif //WITH_ASSIMP

#include <iostream>
#include <stdexcept>
//#include "..\..\nu_utils.h"

namespace SAT
{
	class Model
	{
#ifdef WITH_ASSIMP
	public:
		Model(const char* path);
		~Model();

		template <typename Deprecated_Shader>
		void draw(Deprecated_Shader& shader) const;

		template <typename Deprecated_Shader>
		void drawInstanced(Deprecated_Shader& shader, unsigned int instanceCount);

		void setInstancedModelMatricesData(glm::mat4* modelMatrices, unsigned int count);

		inline const std::vector<LoadedMesh>& getMeshes() const { return meshes; }

	private: //members
		std::vector<LoadedMesh> meshes;
		std::string directory;
		std::vector<MaterialTexture> texturesLoaded;

	private://methods

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		LoadedMesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<MaterialTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
#else //NOT WITH_ASSIMP
		Model(const char* path) {} //do nothing, but still let other things compile -- just fail at runtime.
#endif //WITH_ASSIMP
	};











	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Template Bodies
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_ASSIMP
	template <typename Deprecated_Shader>
	void Model::draw(Deprecated_Shader& shader) const
	{
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			meshes[i].draw(shader);
		}
	}

	template <typename Deprecated_Shader>
	void Model::drawInstanced(Deprecated_Shader& shader, unsigned int instanceCount)
	{
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			meshes[i].drawInstanced(shader, instanceCount);
		}
	}
#endif //WITH_ASSIMP
}
#endif //WITH_SAT_DEMO_BUILD
