#pragma once

#include "SceneNode.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Framework/InteractableDemoBase.h"


using namespace Engine;
using namespace TutorialEngine;

class GeometricAlgebraDemo : public TutorialEngine::InteractableDemoBase
{

protected:
	virtual void init() override;
	virtual void render_game(float dt_sec) override;
	virtual void render_UI() override;
	virtual void tick(float dt_sec) override;
	virtual void gatherInteractableCubeObjects(std::vector<const TriangleList_SNO*>& objectList) override;

};