#pragma once
#include <memory>
#include "SceneNode.h"
#include "Transform.h"
#include "Utils/TutorialRayUtils.h"
#include "Event.h"
#include "VisualVector.h"
#include "Framework/InteractableDemoBase.h" //TriangleList_SNO


namespace TutorialEngine
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// A triangle list that has extra data for making interaction influence a clickable visual vector
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct VectorCollisionTriangleList
		: public TriangleList_SNO
	{
		VectorCollisionTriangleList();
		glm::mat4 cachedPreviousXform;
		bool bRepresentsTip = false;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// scene wrapper around vector collision triangle list
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class SceneNode_VectorEnd : public SceneNode_TriangleList
	{
	public:
		SceneNode_VectorEnd();
	public:
		const VectorCollisionTriangleList& getTriangleList() { return *myTriangleBox; }
	public:
		struct ClickableVisualVector* owner = nullptr;
	private:
		VectorCollisionTriangleList* myTriangleBox;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Visual vector that allows user interaction with its ends
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct ClickableVisualVector :
		public VisualVector,
		//public std::enable_shared_from_this<ClickableVisualVector>,
		public Engine::IEventSubscriber
	{
		ClickableVisualVector();
		ClickableVisualVector(const ClickableVisualVector& copy);
		ClickableVisualVector& operator=(const ClickableVisualVector& copy);
		//ClickableVisualVector(ClickableVisualVector&& move) = delete;
		//ClickableVisualVector& operator=(ClickableVisualVector&& move) = delete;
		virtual void onValuesUpdated(const VisualVector::POD& values) override;
		sp<ClickableVisualVector> getShared() { return sp_this(); }
	protected:
		virtual void postConstruct() override;
	private:
		void sharedInit();

		void handleStartDirty();
		void handleEndDirty();
		void handleStartCollisionUpdated();
		void handleEndCollisionUpdated();
	public:
		sp<SceneNode_VectorEnd> startCollision;
		sp<SceneNode_VectorEnd> endCollision;
		//float tipCollisionCorrectionDistance = 0.5f;
		Engine::Event<const ClickableVisualVector&> eventValuesUpdated;
	private:
		bool bUpdatingFromSceneNode = false;
	};
}