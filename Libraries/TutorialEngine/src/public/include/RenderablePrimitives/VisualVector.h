#pragma once
#include <optional>

#include "Transform.h"
#include "GameObjectBase.h"


//forward declarations
namespace Engine { class Shader;}
namespace Engine { class ConeMesh_GpuBuffer; }
namespace TutorialEngine { struct LineRenderer;}

namespace TutorialEngine
{

	class VisualVector : public Engine::GameObjectBase
	{
	public:
		VisualVector();
		~VisualVector();

		/** Converting to entity so move/copy semantics can probably be disregarded*/
		VisualVector(const VisualVector& copy);
		VisualVector& operator=(const VisualVector& copy);
		VisualVector(VisualVector&& move) noexcept;
		VisualVector& operator=(VisualVector&& move) noexcept;
	public:
		void render(const glm::mat4& projection_view, std::optional<glm::vec3> cameraPos) const;
		void setVector(glm::vec3 newVec);
		void setStart(glm::vec3 newStart);
		void setEnd(glm::vec3 newEnd);

		glm::vec3 getVec() const { return pod.dir; }
		glm::vec3 getStart() const { return pod.startPos; }
	private:
		void updateCache();
	public:
		glm::vec3 color{ 1.f };
		bool bUseCenteredMesh = true;
		struct POD
		{
			glm::vec3 startPos{ 0.f };
			glm::vec3 dir_n{ 1.f,0.f,0.f };
			glm::vec3 dir{ 1.f,0.f,0.f };
			glm::mat4 cachedTipXform{ 1.f };
			glm::mat4 cachedNormalMatrix{ 1.f };
		};
	private:
		POD pod;
		virtual void onValuesUpdated(const struct POD& /*values*/) {};
	private:
		static int numInstances;
		//static sp<StaticMesh::Model> tipMesh; todo_convert_this_to_NOT_use_static_mesh;
		//static sp<StaticMesh::Model> tipMeshOffset; #todo #static_mesh_refactor
		static sp<Engine::ConeMesh_GpuBuffer> tipMesh;
		static sp<Engine::ConeMesh_GpuBuffer> tipMeshOffset;
		static sp<Engine::Shader> tipShader;
		static sp<TutorialEngine::LineRenderer> lineRenderer;
	};


}