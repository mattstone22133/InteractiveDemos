#pragma once

#include <cstdint>

#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"
#include "GameObjectBase.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"
#include "TutorialRayUtils.h"
#include "Utils/MathUtils.h"


namespace Engine {class Shader; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A debug utility
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace TutorialEngine
{
	class TriangleListDebugger : public Engine::GPUResource
		, public Engine::IRemoveCopies
		, public Engine::IRemoveMoves
	{
	public:
		TriangleListDebugger(const TriangleList& list);
		virtual ~TriangleListDebugger();

	public:
		void render(const glm::mat4& projection_view, const glm::mat4& model);
		void bindVBO();
		void configureVBOAttributes();
	protected:
		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;
	private:
		/*GLsizei */ int numVerts = 0;
		/*GLuint */ uint32_t vao, vbo;
		uint32_t attribLoc_vboPositions = 0;
		TriangleList triList;
		sp<Engine::Shader> shader;
	};
}
