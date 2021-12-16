#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <stdint.h>

#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"

namespace Engine { class Shader; }

namespace TutorialEngine
{
	struct LineRenderer final : public Engine::GPUResource,
		public Engine::IRemoveCopies,
		public Engine::IRemoveMoves
	{
	public:
		LineRenderer();

		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;

		void bindVBOs();
		void configureVBOs();

		void renderLine(const glm::vec3& pntA, const glm::vec3& pntB, const glm::vec3& color, const glm::mat4& projection_view);

	public:
		sp<Engine::Shader> shader = nullptr;
		///*GLuint*/ uint32_t vao = 0;
		/*GLuint*/ uint32_t vboPositions = 0;
		uint32_t vboPos_AttribLoc = 0;
	};

}