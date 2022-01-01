#pragma once
#include <cstdint>
#include <optional>

#include <glm/gtc/type_precision.hpp>

#include "Framework/Cameras/CameraInterface.h"
#include "GameObjectBase.h"
#include "Event.h"
#include "SceneNode.h"
#include "Utils/TutorialRayUtils.h"
#include "RenderablePrimitives/LineRenderer.h"
#include "RenderablePrimitives/PlaneRenderer.h"
#include "Utils/TriangleListDebugger.h"
#include "EngineSystems/RenderSystem/RenderSystem.h"


namespace Engine { class FrameRenderData; }

namespace TutorialEngine
{
	class SceneNode_TriangleList;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TriangleList_SNO - Triangle list managed by a scene node for the benefits a scene node provides
	//	SNO = scene node owned
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct TriangleList_SNO : public TriangleList
	{
		TriangleList_SNO(const std::vector<Triangle>& inLocalTriangles) : TriangleList(inLocalTriangles) {}
		virtual ~TriangleList_SNO() {}

		SceneNode_TriangleList* owner = nullptr;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TriangleListSceneNode  - a scene node that managed a triangle list that can be detected with ray casting
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class SceneNode_TriangleList : public Engine::SceneNode
	{
	public:
		SceneNode_TriangleList(const sp<TriangleList_SNO>& inTriangleList)
			: Engine::SceneNode(), myTriangleList(inTriangleList)
		{
			assert(myTriangleList);
			myTriangleList->owner = this;
		}
		virtual void setLocalPosition(const glm::vec3& pos);
		virtual void setLocalRotation(const glm::quat newLocalRotQuat);
		virtual void setLocalScale(const glm::vec3& newScale);
		virtual void setLocalTransform(const Engine::Transform& newTransform);
		virtual void v_CleanComplete();
		const TriangleList_SNO& getTriangleList();
	private:
		void transformTriangleList(const glm::mat4& worldMat);
	protected:
		sp<TriangleList_SNO> myTriangleList = nullptr;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// InteractableDemo - A demo that has built in support for mouse interaction of moving scene node based objects
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class InteractableDemoBase : public Engine::SceneNode
	{
	public:
		InteractableDemoBase();
		InteractableDemoBase(const InteractableDemoBase& copy) = delete;
		InteractableDemoBase(InteractableDemoBase&& move) = delete;
		InteractableDemoBase& operator=(const InteractableDemoBase& copy) = delete;
		InteractableDemoBase& operator=(InteractableDemoBase&& move) = delete;
		virtual ~InteractableDemoBase();
		virtual void postConstruct() override;
	public: //api
		virtual void init();
		virtual void render_game(float dt_sec);
		virtual void render_UI();
		virtual void inputPoll(float dt_sec);
		virtual void tick(float dt_sec);
	public:
		std::optional<glm::vec3> getDrawLineStart();
		std::optional<glm::vec3> getDrawLineEnd();
		std::optional<glm::vec3> getDrawPoint();
	protected:
		/** override this to provide the list of objects you want to be tested when ray casting*/
		virtual void gatherInteractableCubeObjects(std::vector<const TriangleList_SNO*>& objectList) = 0;
		virtual void handleCursorPositionUpdated(double x, double y);
	protected://state
		const TriangleList_SNO* activeClickTarget = nullptr; //#TODO perhaps this would be cleaner to just cache the scene node?
		std::optional<Ray> previousRayCast;
	public://debug
		bool bDrawDebugCubes = false;
		bool bDebugLastRay = false;
		bool bDrawInteractionPlane = false;
		bool bEnableDebugUI = false;
	protected:
		bool bInitOnPostConstruct = true;
		bool bRegisterEditorUiOnPostConstruct = true;
		bool bRenderLineGeneration = true;
		float lineCreationDistFromCamera = 5.0f;
		float lineCreationDistAdjustSpeedSec = 5.0f;
	private: //statics
		static sp<TutorialEngine::TriangleListDebugger> debugCubeRenderer;
		static sp<TutorialEngine::LineRenderer> lineRenderer;
		static sp<TutorialEngine::PlaneRenderer> planeRenderer;
		static int32_t instanceCount;
	private:
		bool bSelectButtonPressed = false;
		glm::dvec2 lastMousePos{ 0.f,0.f };
	private: //implementation details
		std::vector<sp<Engine::SceneNode>> nodePool;
		Engine::SceneNode* start_linePnt = nullptr;
		Engine::SceneNode* end_linePnt = nullptr;
		Engine::SceneNode* customPoint = nullptr;
	};
}
