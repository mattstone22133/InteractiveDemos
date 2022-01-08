#pragma once

#include "SceneNode.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Framework/InteractableDemoBase.h"

namespace TutorialEngine { class VectorProjectionAnimation; }
namespace TutorialEngine { class VisualVector; }
namespace TutorialEngine { class VisualPoint; }
namespace TutorialEngine { struct ClickableVisualPoint; }
namespace TutorialEngine { struct ImmediateTriangle; }
namespace Engine { struct TextBlockSceneNode; }
namespace Engine { class Montserrat_BMF; }

using namespace Engine;
using namespace TutorialEngine;

namespace EBarycentricMode
{
	enum Type {
		INTUITIVE_METHOD = 0,
		OPTIMIZED_PROJECTION,
		AREA_METHOD,
		LINEAR_SYSTEMS_METHOD
	};
}


struct BarycentricsDemo : public TutorialEngine::InteractableDemoBase
{
public:
	static glm::vec3 calcBarycentrics_myMethod(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC);
	static glm::vec3 calcBarycentrics_optimizedProjection(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC);
	static glm::vec3 calcBarycentrics_AreaMethod(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC);
	static glm::vec3 calcBarycentrics_LinearSystemMethod(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC);
protected:
	virtual void init() override;
	virtual void render_game(float dt_sec) override;
	void renderGame_Barycentric_myMethod(float dt_sec);
	void renderGame_Barycentric_OptimizedProjectionMethod(float dt_sec);
	void renderGame_Barycentric_AreaMethod(float dt_sec);
	void renderGame_Barycentric_SolvingLinearSystem(float dt_sec);
	virtual void render_UI() override;
	virtual void tick(float dt_sec) override;
	virtual void gatherInteractableCubeObjects(std::vector<const TriangleList_SNO*>& objectList) override;
	void handleTestPointUpdated(const TutorialEngine::VisualPoint& pnt);
private:
	//avoiding using anim helpers for these atm because helpers cannot be done in single line, after refactoring them will use those instead of having explicit methods below
	//also avoiding because reworked some logic with projection updates to be non-negative language (reset anim instead of talkinga bout test opint updated)
	void helper_renderVector(bool bShouldRender, glm::vec3 start, glm::vec3 dir, glm::vec3 color);
	void helper_renderVector(bool bShouldRender, glm::vec3 start, glm::vec3 dir, glm::vec3 color, float timestampSecs, bool bDriftToOrigin = false);
	void helper_renderProjection(bool bShouldRender, TutorialEngine::VectorProjectionAnimation& projAnim, glm::vec3 aVec, glm::vec3 bVec, glm::vec3 aStart, glm::vec3 bStart, float dt_sec, std::optional<glm::vec3> color = std::nullopt);
	void helper_renderCrossArea(bool bShouldRender, glm::vec3 first, glm::vec3 second, glm::vec3 start, glm::vec3 color, float timestamp_start);
private:
	sp<TutorialEngine::ImmediateTriangle> triRender = nullptr;
	sp<TutorialEngine::VisualVector> genericVector = nullptr;
	sp<TutorialEngine::VisualPoint> genericPoint = nullptr;
	sp<Engine::TextBlockSceneNode> text = nullptr;
	sp<Engine::Montserrat_BMF> font = nullptr;
private:
	bool bDraggableTestPointUpdated = false;
private:
	EBarycentricMode::Type barymode = EBarycentricMode::INTUITIVE_METHOD;


	//ground truths
	bool bRenderRealTimeCollisionBook = false;
	bool bRenderShirleyVersion = false;
private:
	////////////////////////////////////////////////////////
	// shared between all methods
	////////////////////////////////////////////////////////
	bool bWireframe = false;
	bool bRenderBarycentricA = true;
	bool bRenderBarycentricB = false;
	bool bRenderBarycentricC = false;
	////////////////////////////////////////////////////////
	// slow intuitive method flags
	////////////////////////////////////////////////////////
	bool bRenderBToA = false;
	bool bRenderBToC = false;
	bool bRenderBCProj = false;
	bool bRender_EdgeProjectPointToTestPoint = false;
	bool bRender_PerpendicularToEdge = false;
	bool bRenderTestPointProjectionOntoPerpendicular = false;

	float timestamp_RenderBToA = 0.f;
	float timestamp_RenderBToC = 0.f;
	float timestamp_RenderBCProj = 0.f;
	float timestamp_Render_EdgeProjectPointToTestPoint = 0.f;
	float timestamp_Render_PerpendicularToEdge = 0.f;
	float timestamp_RenderTestPointProjectionOntoPerpendicular = 0.f;
	float timestamp_RenderBarycentricA = 0.f;
	float timestamp_RenderBarycentricB = 0.f;
	float timestamp_RenderBarycentricC = 0.f;

	////////////////////////////////////////////////////////
	// optimized projection method flags
	////////////////////////////////////////////////////////
	bool bRenderAB = false;
	bool bRenderCB = false;
	bool bRender_AtoTestPnt = false;
	bool bRender_ProjToCB = false;
	bool bRender_VectorFromFirstProjection = false;
	bool bRender_projTestPointOntoPerpendicular = false;
	bool bRender_projABontoPerpendicular = false;

	float timestamp_renderbRenderAB = 0.f;
	float timestamp_renderbRenderCB = 0.f;
	float timestamp_renderbRender_AtoTestPnt = 0.f;
	float timestamp_renderbRender_Perpendicular = 0.f;
	float timestamp_renderbRender_VectorFromFirstProjection = 0.f;
	float timestamp_renderbRender_projTestPointOntoPerpendicular = 0.f;
	float timestamp_renderbRender_projABontoPerpendicular = 0.f;

	////////////////////////////////////////////////////////
	// area method
	////////////////////////////////////////////////////////
	bool bRenderHalfAreas = false;
	bool bRenderCrossVec_first = false;
	bool bRenderCrossVec_second = false;

	bool bAreaMethod_RenderTriNormals = false;
	bool bRenderCrossProductVectors = false;
	bool bAreaMethod_normalizeNormals = false;
	bool bAreaMethod_renderFullArea = false;
	bool bAreaMethod_renderPBC_Area = false;
	bool bAreaMethod_renderPCA_Area = false;
	bool bAreaMethod_renderPAB_Area = false;


	float timestamp_area_normals = 0.f;
	float timestamp_area_fullarea = 0.f;
	float timestamp_area_PBC_area = 0.f;
	float timestamp_area_PCA_area = 0.f;
	float timestamp_area_PAB_area = 0.f;
	float timestamp_crossvecfirst = 0.f;
	float timestamp_crossvecsecond = 0.f;
	float timestamp_crossproductVecs = 0.f;

private:
	float tickedTime = 0.f;
	float vectorAnimSecs = 1.0f;
private:
	sp<TutorialEngine::ClickableVisualPoint> pntA = nullptr;
	sp<TutorialEngine::ClickableVisualPoint> pntB = nullptr;
	sp<TutorialEngine::ClickableVisualPoint> pntC = nullptr;

	sp<TutorialEngine::ClickableVisualPoint> testPoint = nullptr;

	/////////////////////////////////////////////////////////////////////////////////////
	// intuitive method
	/////////////////////////////////////////////////////////////////////////////////////
	sp<TutorialEngine::VectorProjectionAnimation> projAnim_BC;
	sp<TutorialEngine::VectorProjectionAnimation> projAnim_PointOnPerpendicular;

	////////////////////////////////////////////////////////
	// optimized projection method
	////////////////////////////////////////////////////////
	sp<TutorialEngine::VectorProjectionAnimation> projAnim_ab_onto_cb;
	sp<TutorialEngine::VectorProjectionAnimation> projAnim_testPointOnPerpendicular;
	sp<TutorialEngine::VectorProjectionAnimation> projAnim_aBOnPerpendicular;
};