#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cstdint>
#include <vector>

#include "GameObjectBase.h"
#include "Utils/MathUtils.h"
#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"

namespace Engine
{
	class Shader;
}

namespace TutorialEngine
{
	struct PlaneRenderer final : public Engine::GPUResource
		, public Engine::IRemoveCopies
		, public Engine::IRemoveMoves
	{
	public:
		PlaneRenderer();
		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;
		void renderPlane(const glm::vec3& centerPnt, const glm::vec3& normal, const glm::vec3& scale, const glm::vec4& color, const glm::mat4& projection_view);
	private:
		void bindVBO();
		void configureVBO();
	public:
		bool bScreenDoorEffect = false;
		sp<Engine::Shader> shader = nullptr;
		///*GLuint*/ uint32_t vao = 0;
		/*GLuint */uint32_t vboPositions = 0;
		uint32_t attribLoc_vboPositions = 0;
	};

}