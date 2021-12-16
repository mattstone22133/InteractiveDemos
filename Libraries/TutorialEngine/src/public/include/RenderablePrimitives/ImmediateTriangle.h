#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "GameObjectBase.h"
#include <vector>

#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"

namespace Engine { class Shader;}

namespace TutorialEngine
{
	struct ImmediateTriangle final : public Engine::GPUResource
		, public Engine::IRemoveCopies
		, public Engine::IRemoveMoves
	{
	public:
		void renderTriangle(const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& pntC, const glm::vec3& color, const glm::mat4& projection_view);
	private:
		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;

		void bindVBO();
		void configureVBO();

	public:
		sp<Engine::Shader> shader = nullptr;
		///*GLuint*/ uint32_t vao = 0;
		/*GLuint*/ uint32_t vboPositions = 0;
		uint32_t vboPos_attribLoc = 0;
		bool bWireFrame = false;
	};

}