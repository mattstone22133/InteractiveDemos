#pragma once

#include <memory>

#include "RenderablePrimitives/ClickableVisualVector.h"

#include "SceneNode.h"
#include "Transform.h"
#include "Event.h"
#include "Utils/Shader.h"
#include "Utils/TutorialRayUtils.h"
#include "RenderablePrimitives/VisualVector.h"
#include "RenderablePrimitives/VisualPoint.h"

/** This is a quick copy paste of the visual vector class -- refer to that for questions/comments about implementation */
namespace TutorialEngine
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// A triangle list that has extra data for making interaction influence a clickable visual vector
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct PointCollisionTriangleList
		: public TriangleList_SNO
	{
		PointCollisionTriangleList();
		glm::mat4 cachedPreviousXform;
		bool bRepresentsTip = false;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// scene wrapper around vector collision triangle list
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class SceneNode_Point : public SceneNode_TriangleList
	{
	public:
		SceneNode_Point();
	public:
		const PointCollisionTriangleList& getTriangleList() { return *myTriangleBox; }
	public:
		struct ClickableVisualPoint* owner = nullptr;
	private:
		PointCollisionTriangleList* myTriangleBox;
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Visual vector that allows user interaction with its ends
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct ClickableVisualPoint :
		public TutorialEngine::VisualPoint
		//public std::enable_shared_from_this<ClickableVisualPoint>,
	{
		ClickableVisualPoint();
		//ClickableVisualPoint(const ClickableVisualPoint& copy);
		//ClickableVisualPoint& operator=(const ClickableVisualPoint& copy);
		virtual void onValuesUpdated(const TutorialEngine::VisualPoint::POD& values) override;
		sp<ClickableVisualPoint> getShared() { return sp_this(); }
	protected:
		virtual void postConstruct() override;
	private:
		void sharedInit();
		void handleStartDirty();
		void handlePointCollisionUpdated();
	public:
		sp<TutorialEngine::SceneNode_Point> pointCollision;
	private:
		bool bUpdatingFromSceneNode = false;
	};
};