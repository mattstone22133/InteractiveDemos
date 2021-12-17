#pragma once
#include <optional>

#include "Transform.h"
#include "GameObjectBase.h"
#include "Event.h"
#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"

namespace Engine{ class Shader;}
namespace Engine { class SphereMesh_GpuBuffer; }
namespace TutorialEngine{struct LineRenderer;}

namespace TutorialEngine
{
	/** This is a quick copy-paste modification of visual vector, if anything appears strange look at that class for reference. */
	class VisualPoint : public Engine::GPUResource
	{
	public:
		VisualPoint();
		~VisualPoint();

		/** Converting to entity so move/copy semantics cache probably be disregarded*/
		VisualPoint(const VisualPoint& copy);
		VisualPoint& operator=(const VisualPoint& copy);
		VisualPoint(VisualPoint&& move); 
		VisualPoint& operator=(VisualPoint&& move);
	public:
		void render(const glm::mat4& projection_view, std::optional<glm::vec3> cameraPos) const;
		void setPosition(glm::vec3 newStart);
		void setUserScale(glm::vec3 newScale);

		/** Additional scaling is applied, user scale is only a portion of that scaling.*/
		glm::vec3 getUserScale() { return pod.scale; }
		glm::vec3 getPosition() { return pod.position; }
		Engine::Event<const VisualPoint&> eventValuesUpdated;
	private:
		void updateCache();
	public:
		glm::vec3 color{ 1.f };
		struct POD
		{
			glm::vec3 position{ 0.f };
			glm::vec3 scale{ 1.f };
			glm::mat4 cachedXform{ 1.f };
			glm::mat4 cachedNormalMatrix{ 1.f };
		};
	private:
		POD pod;
		virtual void onValuesUpdated(const struct POD& /*values*/) {};
	protected:
		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;
	private:
		static int numInstances;
		static sp<Engine::SphereMesh_GpuBuffer> pointMesh;
		static sp<Engine::Shader> pointShader;
	};

}