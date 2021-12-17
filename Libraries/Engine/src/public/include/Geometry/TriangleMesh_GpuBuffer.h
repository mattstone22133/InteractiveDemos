#pragma once

#include <vector>
#include <cstdint>

#include <Utils/MathUtils.h>
#include <Event.h>
#include <GameObjectBase.h>

#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"

namespace Engine { class Shader; }

namespace Engine
{
	class TriangleMesh_GpuBuffer : public GPUResource
		, public IRemoveMoves
		, public IRemoveCopies
	{
	private:
		bool bBufferedOnGpu = false;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<uint32_t> elementIndices;
		uint32_t vboPositions = 0;
		uint32_t vboNormals = 0;
		uint32_t ebo = 0;
		uint32_t attributeLoc_vboPositions = 0;
		uint32_t attributeLoc_vboNormals = 0;
	public: 
		const char* attributeShaderName_Positions = "position";
		const char* attributeShaderName_Normals = "normal";
		bool bRenderWireframe = false;
	public:
		TriangleMesh_GpuBuffer();
		virtual ~TriangleMesh_GpuBuffer();

		void setPositions(const std::vector<glm::vec3>& positions);
		void setNormals(const std::vector<glm::vec3>& normals);
		void setElementIndices(const std::vector<uint32_t>& elementIndices);

		void cacheAttribLocations(Shader& shader);
		void render(Shader& shader, const bool callUseShader = true);	//reads shader to find attribute locations
		void renderCached();				//renders using cached attrib locations, for efficiency

	protected:
		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;
	};


}