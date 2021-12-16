#include "RenderablePrimitives/ClickableVisualPoint.h"

#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Utils/Shader.h"
#include "GeometryData/TriangleCube.h"

namespace TutorialEngine
{
	PointCollisionTriangleList::PointCollisionTriangleList()
		: TriangleList_SNO(TutorialEngine::TriangleCube{}.triangles)
	{

	}

	SceneNode_Point::SceneNode_Point()
		: SceneNode_TriangleList(new_sp<PointCollisionTriangleList>())
	{
		myTriangleBox = static_cast<PointCollisionTriangleList*>(myTriangleList.get());
		assert(myTriangleBox);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	void ClickableVisualPoint::sharedInit()
	{
		pointCollision = new_sp<SceneNode_Point>();

		//not well enough encapsulated
		pointCollision->owner = this;
	}

	void ClickableVisualPoint::onValuesUpdated(const TutorialEngine::VisualPoint::POD& values)
	{
		//prevent recursion since scene node responds to vector updates. Don't have scene nodes update when we're retro-applying an update from the scene node itself (eg clicking and dragging a vector)
		if (!bUpdatingFromSceneNode)
		{
			Engine::Transform xform;
			xform.scale = glm::vec3(0.25f);
			xform.position = values.position;
			pointCollision->setLocalTransform(xform);
		}
	}


	void ClickableVisualPoint::postConstruct()
	{
		pointCollision->dirtyEvent.addWeakObj(sp_this(), &ClickableVisualPoint::handleStartDirty);

		pointCollision->updatedEvent.addWeakObj(sp_this(), &ClickableVisualPoint::handlePointCollisionUpdated);
	}


	void ClickableVisualPoint::handleStartDirty()
	{
		pointCollision->requestClean();
	}


	void ClickableVisualPoint::handlePointCollisionUpdated()
	{
		bUpdatingFromSceneNode = true;
		setPosition(pointCollision->getWorldPosition());
		bUpdatingFromSceneNode = false;
	}

	ClickableVisualPoint::ClickableVisualPoint()
	{
		sharedInit();
	}

	//ClickableVisualPoint::ClickableVisualPoint(const VisualPoint& copy)
	//{
	//	if (&copy != this)
	//	{
	//		sharedInit();
	//		pointCollision->setLocalTransform(copy.pointCollision->getLocalTransform());
	//	}
	//}

	//ClickableVisualPoint& ClickableVisualPoint::operator=(const VisualPoint& copy)
	//{
	//	if (&copy != this)
	//	{
	//		sharedInit();
	//		pointCollision->setLocalTransform(copy.pointCollision->getLocalTransform());
	//	}
	//	return *this;
	//}

}