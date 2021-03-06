#include <iostream>
#include <TutorialEngine.h>
#include <GameObjectBase.h>

#include "BarycentricCoordinateDemo.h"
#include "Framework/Cameras/QuaternionCamera.h"
#include "RenderablePrimitives/ClickableVisualPoint.h"
#include "RenderablePrimitives/ImmediateTriangle.h"
#include "RenderablePrimitives/VectorProjectionAnimation.h"
#include "Tools/Text/BitmapFont/BitmapFontBase.h"
#include "Tools/Text/BitmapFont/Montserrat_BMF.h"
#include "Utils/MathUtils.h"

#if WITH_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif //WITH_IMGUI

using namespace Engine;
using namespace TutorialEngine;
using namespace MathUtils;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float clampPerc(float start, float now, float max)
{
	float perc = glm::clamp<float>((now - start) / max, 0.f, 1.f);
	return perc;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// barycentric coordinates 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

glm::vec3 Shirley_getBarycentricCoordinatesAt(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	//Graphics book Shirly method for calculating barycentrics using areas from cross product: https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
	//tri is formed by a,b,c
	using namespace glm;

	glm::vec3 bary;
	glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));

	// The area of a triangle is 
	float areaABC = glm::dot(normal, glm::cross((b - a), (c - a)));
	float areaPBC = glm::dot(normal, glm::cross((b - p), (c - p)));
	float areaPCA = glm::dot(normal, glm::cross((c - p), (a - p)));

	bary.x = areaPBC / areaABC; // alpha
	bary.y = areaPCA / areaABC; // beta
	bary.z = 1.0f - bary.x - bary.y; // gamma

	return bary;
}


void realtimecollisiondetectionbook_Barycentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float& u, float& v, float& w)
{
	glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = glm::dot(v0, v0);
	float d01 = glm::dot(v0, v1);
	float d11 = glm::dot(v1, v1);
	float d20 = glm::dot(v2, v0);
	float d21 = glm::dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	v = (d11 * d20 - d01 * d21) / denom;
	w = (d00 * d21 - d01 * d20) / denom;
	u = 1.0f - v - w;
}

glm::vec3 BarycentricsDemo::calcBarycentrics_myMethod(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC)
{
	using namespace glm;

	////////////////////////////////////////////////////////
	//calculate barycentric coordinates of test point
	////////////////////////////////////////////////////////
	const auto calcBarycentric = [&](
		const glm::vec3& aPos
		, const glm::vec3& bPos
		, const glm::vec3& cPos
		)
	{
		const vec3 b_to_a = aPos - bPos;
		const vec3 b_to_c = cPos - bPos;
		const vec3 bc_proj = projectAontoB(b_to_a, b_to_c);
		const vec3 projectionPoint = bPos + bc_proj;
		const vec3 perpendicularWithEdgeVec = aPos - projectionPoint; //perpendicular line to triangle edge
		const vec3 edgeProjPoint_to_TestPoint = testPoint - projectionPoint;
		const vec3 testPoint_projOnTo_perpendicular = projectAontoB(edgeProjPoint_to_TestPoint, perpendicularWithEdgeVec);

		//calculate the barycentric, which is lengthRatio.
		const float scalarProj = glm::dot(edgeProjPoint_to_TestPoint, glm::normalize(perpendicularWithEdgeVec));
		const float  lengthRatio = scalarProj / glm::length(perpendicularWithEdgeVec);

		return lengthRatio;
	};

	const float barycentric_a = calcBarycentric(pntA, pntB, pntC);
	const float barycentric_b = calcBarycentric(pntB, pntC, pntA);
	const float barycentric_c = calcBarycentric(pntC, pntA, pntB);

	return glm::vec3(barycentric_a, barycentric_b, barycentric_c);
}


glm::vec3 BarycentricsDemo::calcBarycentrics_optimizedProjection(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Below can be highly optimized by analyzing the actual math symbolically and removing redundancies in calculating projections etc.
	// (see Math for Game Developers - Ray Triangle Intersection by Jorge Rodriguez). I'm more interested
	// in teaching the concept rather than every exact method that has been optimized completely.
	// I may look and see what what this all reduces down to and show it, but at the moment leaving an unoptimized and untested version commented out
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace glm;

	auto calcBarycentric = [&](
		const vec3& aPos
		, const vec3& bPos
		, const vec3& cPos
		)
	{
		//notes:
		// The math here doesn't fully represent the animation. 
		// It has had some elements removed out for simplification. I left those removed elements commented out so you can see their origins
		// This is why we do the projections manually here, rather than calling the helper function.
		// The projections have had their divide step removed, because the result is the same without it. 
		const vec3 a_to_b = bPos - aPos;
		const vec3 b_to_c = cPos - bPos;
		const vec3 a_to_testPoint = testPoint - aPos;

		const vec3 aToB_projection_OnToBtoC = (glm::dot(a_to_b, b_to_c) / glm::dot(b_to_c, b_to_c)) * b_to_c;
		const vec3 perpendicular_vector = a_to_b - aToB_projection_OnToBtoC; //gray in animation

		const float proj_a_to_testpointFactor = glm::dot(a_to_testPoint, perpendicular_vector) /* /dot(perpendicular_vector,perpendicular_vector)*/;
		const float proj_ab = glm::dot(a_to_b, perpendicular_vector) /* /dot(perpendicular_vector,perpendicular_vector)*/;

		return 1 - (proj_a_to_testpointFactor / proj_ab);
	};

	float barycentric_a = calcBarycentric(pntA, pntB, pntC);
	float barycentric_b = calcBarycentric(pntB, pntC, pntA);
	float barycentric_c = 1 - (barycentric_a + barycentric_b);

	return glm::vec3(barycentric_a, barycentric_b, barycentric_c);
}

glm::vec3 BarycentricsDemo::calcBarycentrics_AreaMethod(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC)
{
	//Graphics book Shirley method for calculating barycentrics using areas from cross product: https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
	//triangle is formed by a,b,c
	using namespace glm;

	//const glm::vec3 normal = glm::normalize(glm::cross(pntB - pntA, pntC - pntA)); //normalized, but we don't have to do this it appears, skipping divide more efficient
	const glm::vec3 normal = glm::cross(pntB - pntA, pntC - pntA);//it appears this will work even if normal isn't normalized, but areas apear to be scalar projections so this may be invalid in terms of correct area, but still works as it is same proportion?

	// The area of a triangle is below (note: we are using the dot product with normal to get length from cross result)
	const float areaABC = glm::dot(normal, glm::cross((pntB - pntA), (pntC - pntA)));
	const float areaPBC = glm::dot(normal, glm::cross((pntB - testPoint), (pntC - testPoint)));
	const float areaPCA = glm::dot(normal, glm::cross((pntC - testPoint), (pntA - testPoint)));

	//we can derive third barycentric, so no need to calculate PAB.

	glm::vec3 barycentric;
	barycentric.x = areaPBC / areaABC; // alpha
	barycentric.y = areaPCA / areaABC; // beta
	barycentric.z = 1.0f - barycentric.x - barycentric.y; // gamma

	return barycentric;
}

glm::vec3 BarycentricsDemo::calcBarycentrics_LinearSystemMethod(glm::vec3 testPoint, glm::vec3 pntA, glm::vec3 pntB, glm::vec3 pntC)
{
	//real time collision (book) method using cramer's rule to solve system of equations for barycentric coordinates.

	glm::vec3 a_to_b = pntB - pntA;
	glm::vec3 a_to_c = pntC - pntA;
	glm::vec3 a_to_testpoint = testPoint - pntA;

	float d00 = glm::dot(a_to_b, a_to_b);
	float d01 = glm::dot(a_to_b, a_to_c);
	float d11 = glm::dot(a_to_c, a_to_c);
	float d20 = glm::dot(a_to_testpoint, a_to_b);
	float d21 = glm::dot(a_to_testpoint, a_to_c);

	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	return glm::vec3(u, v, w);
}

void BarycentricsDemo::init()
{
	InteractableDemoBase::init();

	bRenderLineGeneration = false; //stop rendering rend line when clicking on nothing

	// set up camera for viewing
	sp<Engine::QuaternionCamera> quatCam = new_sp<Engine::QuaternionCamera>();
	quatCam->pos = glm::vec3(0, 0, 5.f);
	RenderSystem::get().setRenderCamera(quatCam);

	triRender = new_sp<TutorialEngine::ImmediateTriangle>();
	font = new_sp<Engine::Montserrat_BMF>("./PreloadAssets/TutorialEngine/font/Montserrat_ss_alpha_1024x1024_wb.png");
	
	pntA = new_sp<TutorialEngine::ClickableVisualPoint>();
	pntB = new_sp<TutorialEngine::ClickableVisualPoint>();
	pntC = new_sp<TutorialEngine::ClickableVisualPoint>();
	testPoint = new_sp<TutorialEngine::ClickableVisualPoint>();

	glm::vec3 triPoint_A = glm::vec3(-1, -1, -1); //left
	glm::vec3 triPoint_B = glm::vec3(1, -1, -1); //right
	glm::vec3 triPoint_C = glm::vec3(0, 1, -1); //top
	pntA->setPosition(triPoint_A);
	pntB->setPosition(triPoint_B);
	pntC->setPosition(triPoint_C);
	

	//doing some tests to see if calculations match positions
	//testPoint->setPosition(0.33f*triPoint_A + 0.33f*triPoint_B + 0.33f*triPoint_C);
	//testPoint->setPosition(0.66f*triPoint_A + 0.0f*triPoint_B + 0.33f*triPoint_C); //matches drag position
	//testPoint->setPosition(0.89f*triPoint_A + 1.17f*triPoint_B + 0.72f*triPoint_C); // does not match (doesn't sum to 1?
	//testPoint->setPosition(0.19f*triPoint_A + 0.71f*triPoint_B + 0.009f*triPoint_C);
	//testPoint->setPosition(0.21f*triPoint_A + 0.73f*triPoint_B + 0.05f*triPoint_C);
	//testPoint->setPosition(0.674f*triPoint_A + 0.198f*triPoint_B + 0.128f*triPoint_C);  //exact match
	//testPoint->setPosition(0.224f*triPoint_A + 0.517f*triPoint_B + 0.260f*triPoint_C); //exact match
	//testPoint->setPosition(0.08f*triPoint_A + 0.152f*triPoint_B + 0.768f*triPoint_C); 
	//testPoint->setPosition(-0.112f*triPoint_A + 0.629f*triPoint_B + 0.484f*triPoint_C);  //test using a negative weight
	//testPoint->setPosition(-0.369f*triPoint_A + 1.1139f*triPoint_B + 0.229f*triPoint_C);   //appears  to match fairly close
	testPoint->setPosition(1.174f * triPoint_A + -0.483f * triPoint_B + 0.309f * triPoint_C);  //appears to match, this is a good example point

	genericVector = new_sp<TutorialEngine::VisualVector>();
	genericVector->bUseCenteredMesh = false;
	genericPoint = new_sp<TutorialEngine::VisualPoint>();
	text = new_sp<Engine::TextBlockSceneNode>(font, "0.f");

	////////////////////////////////////////////////////////
	// my method
	////////////////////////////////////////////////////////
	projAnim_BC = new_sp<VectorProjectionAnimation>();
	projAnim_PointOnPerpendicular = new_sp<VectorProjectionAnimation>();

	testPoint->eventValuesUpdated.addWeakObj(sp_this(), &BarycentricsDemo::handleTestPointUpdated);
	pntA->eventValuesUpdated.addWeakObj(sp_this(), &BarycentricsDemo::handleTestPointUpdated);
	pntB->eventValuesUpdated.addWeakObj(sp_this(), &BarycentricsDemo::handleTestPointUpdated);
	pntC->eventValuesUpdated.addWeakObj(sp_this(), &BarycentricsDemo::handleTestPointUpdated);

	////////////////////////////////////////////////////////
	// optimized projection method
	////////////////////////////////////////////////////////
	projAnim_ab_onto_cb = new_sp<VectorProjectionAnimation>();
	projAnim_testPointOnPerpendicular = new_sp<VectorProjectionAnimation>();
	projAnim_aBOnPerpendicular = new_sp<VectorProjectionAnimation>();


	const float ProjectionAnimationDuration = 3.0f;
	projAnim_BC->animDurSec							= ProjectionAnimationDuration;
	projAnim_PointOnPerpendicular->animDurSec		= ProjectionAnimationDuration;
	projAnim_ab_onto_cb->animDurSec					= ProjectionAnimationDuration;
	projAnim_testPointOnPerpendicular->animDurSec	= ProjectionAnimationDuration;
	projAnim_aBOnPerpendicular->animDurSec			= ProjectionAnimationDuration;

	updateUserTypedWeights();
}

void BarycentricsDemo::render_game(float dt_sec)
{
	InteractableDemoBase::render_game(dt_sec);
	using namespace glm;

	const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
	if (rd)
	{
		// RENDER THE TRIANGLE
		triRender->bWireFrame = bWireframe;
		vec3 triOffset = vec3(0.f, 0.f, -0.01f); //make sure green triangle renders behind vectors, be sliding it backwards just a bit.
		triRender->renderTriangle(
			  pntA->getPosition() + triOffset
			, pntB->getPosition() + triOffset
			, pntC->getPosition() + triOffset, glm::vec3(0.0f, 0.5f, 0.0f), rd->projection_view);
		triRender->bWireFrame = false;

		// CALCULATE BARYCENTRICS AND RENDER THE POINTS AND VECTORS
		if (rd->camera)
		{
			glm::vec3 camPos = rd->camera->getPosition();
			pntA->render(rd->projection_view, camPos);
			pntB->render(rd->projection_view, camPos);
			pntC->render(rd->projection_view, camPos);

			testPoint->color = vec3(1.f, 1.f, 0);
			testPoint->render(rd->projection_view, camPos);

			if (QuaternionCamera* camera = dynamic_cast<QuaternionCamera*>(rd->camera.get()))
			{
				auto renderPointText = [&](
					const sp<TutorialEngine::ClickableVisualPoint>& a
					, const sp<TutorialEngine::ClickableVisualPoint>& b
					, const sp<TutorialEngine::ClickableVisualPoint>& c
					, const char* const textStr
					)
				{
					vec3 c_to_a = a->getPosition() - c->getPosition();
					vec3 b_to_a = a->getPosition() - b->getPosition();
					vec3 textOffset = glm::normalize(c_to_a + b_to_a);
					constexpr float textOffsetDist = 0.25f;
					vec3 textPosition = textOffset * textOffsetDist + a->getPosition();

					text->setLocalPosition(textPosition);
					text->setLocalRotation(camera->getRotation());
					text->wrappedText->text = textStr;
					text->setLocalScale(vec3(5.f));
				};
				renderPointText(pntA, pntB, pntC, "A");
				text->render(rd->projection, rd->view);

				renderPointText(pntB, pntC, pntA, "B");
				text->render(rd->projection, rd->view);

				renderPointText(pntC, pntA, pntB, "C");
				text->render(rd->projection, rd->view);

				{
					glm::vec3 barycentrics = vec3(0.f);

					if (barymode == EBarycentricMode::INTUITIVE_METHOD)
					{
						barycentrics = calcBarycentrics_myMethod(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition());
					}
					else if (barymode == EBarycentricMode::OPTIMIZED_PROJECTION)
					{
						barycentrics = calcBarycentrics_optimizedProjection(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition());
					}
					else if (barymode == EBarycentricMode::AREA_METHOD)
					{
						barycentrics = calcBarycentrics_AreaMethod(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition());
					}
					else if (barymode == EBarycentricMode::LINEAR_SYSTEMS_METHOD)
					{
						barycentrics = calcBarycentrics_LinearSystemMethod(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition());
					}
					else
					{
						//default to my method if something went wrong
						barycentrics = calcBarycentrics_myMethod(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition());
					}

					//set the barycentric coordinate text on the test point the user can drag
					char textBuffer[128];
					snprintf(textBuffer, sizeof(textBuffer), "(A: %3.2f, B: %3.2f, C: %3.2f)", barycentrics.x, barycentrics.y, barycentrics.z);
					text->wrappedText->text = std::string(textBuffer);

					const float pointOffsetDis = 0.15f;
					text->setLocalPosition(
						testPoint->getPosition()
						+ camera->getUp() * pointOffsetDis
						+ -camera->getFront() * pointOffsetDis);
					text->wrappedText->bitMapFont->setFontColor(vec3(1.f, 1.f, 0.5f));
					text->setLocalScale(vec3(4.f));
					text->render(rd->projection, rd->view);
					text->wrappedText->bitMapFont->setFontColor(vec3(1.0f));

					//use ground truths from established barycentric formulas to test logic. Render these for visual comparison if something is wrong.
					float movingGroundTruthTextOffset = 0.f; //let multiple truths be displayed by updated this
					if (bRenderShirleyVersion)
					{
						vec3 shirlyBary = Shirley_getBarycentricCoordinatesAt(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition());
						snprintf(textBuffer, sizeof(textBuffer), "(A: %3.2f, B: %3.2f, C: %3.2f)", shirlyBary.x, shirlyBary.y, shirlyBary.z);
						text->wrappedText->text = std::string(textBuffer);
						movingGroundTruthTextOffset += 0.2f;
						glm::vec3 adjustedPosForGroundTruth = text->getLocalPosition() + camera->getUp() * movingGroundTruthTextOffset;
						text->setLocalPosition(adjustedPosForGroundTruth);
						text->render(rd->projection, rd->view);
					}
					if (bRenderRealTimeCollisionBook)
					{
						float u, v, w;
						realtimecollisiondetectionbook_Barycentric(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition(), u, v, w);

						snprintf(textBuffer, sizeof(textBuffer), "(A: %3.2f, B: %3.2f, C: %3.2f)", u, v, w);
						text->wrappedText->text = std::string(textBuffer);
						movingGroundTruthTextOffset += 0.1f;
						glm::vec3 adjustedPosForGroundTruth = text->getLocalPosition() + camera->getUp() * movingGroundTruthTextOffset;
						text->setLocalPosition(adjustedPosForGroundTruth);
						text->render(rd->projection, rd->view);
					}

					// RENDER ANIMATIONS
					if (barymode == EBarycentricMode::INTUITIVE_METHOD)
					{
						renderGame_Barycentric_myMethod(dt_sec);
					}
					else if (barymode == EBarycentricMode::OPTIMIZED_PROJECTION)
					{
						renderGame_Barycentric_OptimizedProjectionMethod(dt_sec);
					}
					else if (barymode == EBarycentricMode::AREA_METHOD)
					{
						renderGame_Barycentric_AreaMethod(dt_sec);
					}
					else if (barymode == EBarycentricMode::LINEAR_SYSTEMS_METHOD)
					{
						renderGame_Barycentric_SolvingLinearSystem(dt_sec);
					}
					else
					{
						renderGame_Barycentric_myMethod(dt_sec);
					}

				}

			}
		}

		//reset on next tick, instead of end of render, so that UI can use this flag 
		bDraggableTestPointUpdated = false; //always reset state of moving a test point, now that we have used this flag in rendering animations (used to update projections).
	}
}

void BarycentricsDemo::renderGame_Barycentric_myMethod(float dt_sec)
{
	using namespace glm;

	const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
	auto renderVectorExplanation = [&](
		const sp<TutorialEngine::ClickableVisualPoint>& a
		, const sp<TutorialEngine::ClickableVisualPoint>& b
		, const sp<TutorialEngine::ClickableVisualPoint>& c)
	{
		if (rd && rd->camera)
		{
			glm::vec3 camPos = rd->camera->getPosition();
			glm::vec3 camOffset = rd->camera->getUp() * 0.01f; //create slight offset so that there isn't zfighting on lines

			const vec3 aPos = a->getPosition();
			const vec3 bPos = b->getPosition();
			const vec3 cPos = c->getPosition();

			const vec3 b_to_a = aPos - bPos;
			helper_renderVector(bRenderBToA, bPos + camOffset, b_to_a, vec3(1.f, 0.f, 0.f), timestamp_RenderBToA);

			const vec3 b_to_c = c->getPosition() - bPos;
			helper_renderVector(bRenderBToC, bPos + camOffset, b_to_c, vec3(0.f, 0.f, 1.f), timestamp_RenderBToC);

			const vec3 bc_proj = projectAontoB(b_to_a, b_to_c);
			helper_renderProjection(bRenderBCProj, *projAnim_BC, b_to_a, b_to_c, bPos + camOffset, bPos + camOffset, dt_sec, glm::vec3(1, 0, 0));

			const vec3 projectionPoint = bPos + bc_proj;
			const vec3 edgeProjPoint_to_TestPoint = testPoint->getPosition() - projectionPoint;
			helper_renderVector(bRender_EdgeProjectPointToTestPoint, projectionPoint + camOffset, edgeProjPoint_to_TestPoint, vec3(1.f, 1.f, 0.f), timestamp_Render_EdgeProjectPointToTestPoint);

			const vec3 perpendicularWithEdgeVec = aPos - projectionPoint; //perpendicular line to triangle edge
			helper_renderVector(bRender_PerpendicularToEdge, projectionPoint + camOffset, perpendicularWithEdgeVec, vec3(0.5f, 0.5f, 0.5f), timestamp_Render_PerpendicularToEdge);

			const vec3 testPoint_projOnTo_perpendicular = projectAontoB(edgeProjPoint_to_TestPoint, perpendicularWithEdgeVec);
			helper_renderProjection(bRenderTestPointProjectionOntoPerpendicular, *projAnim_PointOnPerpendicular, edgeProjPoint_to_TestPoint, perpendicularWithEdgeVec, projectionPoint + camOffset, projectionPoint + camOffset, dt_sec, vec3(1.f, 1.f, 0.f));

			//visual this ratio?
			//TODO this needs to be updated to scalar projection method
			float lengthRatio = glm::length(testPoint_projOnTo_perpendicular) / glm::length(perpendicularWithEdgeVec);
		}
	};

	if (bRenderBarycentricA) { renderVectorExplanation(pntA, pntB, pntC); }
	if (bRenderBarycentricB) { renderVectorExplanation(pntB, pntC, pntA); }
	if (bRenderBarycentricC) { renderVectorExplanation(pntC, pntA, pntB); }
}

void BarycentricsDemo::renderGame_Barycentric_OptimizedProjectionMethod(float dt_sec)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Below can be highly optimized by analyzing the actual math symbolically and removing redundancies in calculating projections etc.
	// (see Math for Game Developers - Ray Triangle Intersection by Jorge Rodriguez). I'm more interested
	// in teaching the concept rather than every exact method that has been optimized completely.
	// I may look and see what what this all reduces down to and show it, but at the moment leaving an unoptimized and untested version commented out
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace glm;

	auto calcBarycentric = [&](
		const vec3& aPos
		, const vec3& bPos
		, const vec3& cPos
		)
	{
		const vec3 ab = bPos - aPos;
		helper_renderVector(bRenderAB, aPos, ab, vec3(1, 0, 0), timestamp_renderbRenderAB, true);

		const vec3 cb = cPos - bPos;
		helper_renderVector(bRenderCB, bPos, cb, vec3(0, 1, 0), timestamp_renderbRenderCB, true);

		const vec3 a_to_testPoint = testPoint->getPosition() - aPos;
		helper_renderVector(bRender_AtoTestPnt, aPos, a_to_testPoint, vec3(1, 1, 0), timestamp_renderbRender_AtoTestPnt, true);

		const vec3 projectionToBuildPerpendicular = ((glm::dot(ab, cb) / glm::dot(cb, cb)) * cb);
		helper_renderProjection(bRender_ProjToCB, *projAnim_ab_onto_cb, ab, cb, vec3(0.f), vec3(0.f), dt_sec, glm::vec3(1.f, 0.f, 0.f));

		glm::vec3 perpendicular = ab - projectionToBuildPerpendicular;
		helper_renderVector(bRender_VectorFromFirstProjection, projectionToBuildPerpendicular, ab - projectionToBuildPerpendicular, vec3(0.5f), timestamp_renderbRender_VectorFromFirstProjection, true);

		float proj_a_to_testpointFactor = glm::dot(a_to_testPoint, perpendicular) /* /dot(perpendicular,perpendicular)*/;
		helper_renderProjection(bRender_projTestPointOntoPerpendicular, *projAnim_testPointOnPerpendicular, a_to_testPoint, perpendicular, vec3(0.f), vec3(0.f), dt_sec);

		float proj_ab = glm::dot(ab, perpendicular) /* /dot(perpendicular,perpendicular)*/;
		helper_renderProjection(bRender_projABontoPerpendicular, *projAnim_aBOnPerpendicular, ab, perpendicular, vec3(0.f), vec3(0.f), dt_sec, vec3(1.f, 0.f, 0.f));

		return 1 - (proj_a_to_testpointFactor / proj_ab);
	};

	if (bRenderBarycentricA) { float barycentric_a = calcBarycentric(pntA->getPosition(), pntB->getPosition(), pntC->getPosition()); }
	if (bRenderBarycentricB) { float barycentric_b = calcBarycentric(pntB->getPosition(), pntC->getPosition(), pntA->getPosition()); }
	if (bRenderBarycentricC) { float barycentric_c = calcBarycentric(pntC->getPosition(), pntA->getPosition(), pntB->getPosition()); }

	//glm::vec3 baryCentrics = glm::vec3(barycentric_a, barycentric_b, barycentric_c);
}

void BarycentricsDemo::renderGame_Barycentric_AreaMethod(float dt_sec)
{
	auto renderCrossVecHelper = [this](bool bShouldRender, glm::vec3 first, glm::vec3 second, glm::vec3 start)
	{
		if (bShouldRender)
		{
			helper_renderVector(bRenderCrossVec_first, start, first, glm::vec3(1, 0, 0), timestamp_crossvecfirst);
			helper_renderVector(bRenderCrossVec_second, start, second, glm::vec3(0, 1, 0), timestamp_crossvecsecond);
		}
	};


	//Graphics book Shirley method for calculating barycentrics using areas from cross product: https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
	//tri is formed by a,b,c
	using namespace glm;
	const vec3 a = pntA->getPosition();
	const vec3 b = pntB->getPosition();
	const vec3 c = pntC->getPosition();
	const vec3 pnt = testPoint->getPosition();

	glm::vec3 bary;
	glm::vec3 normal_v = glm::cross(b - a, c - a);
	glm::vec3 normal_n = glm::normalize(normal_v);
	helper_renderVector(bAreaMethod_RenderTriNormals, a, bAreaMethod_normalizeNormals ? normal_n : normal_v, vec3(0.f, 0.f, 1.f), timestamp_area_normals);
	helper_renderVector(bAreaMethod_RenderTriNormals, b, bAreaMethod_normalizeNormals ? normal_n : normal_v, vec3(0.f, 0.f, 1.f), timestamp_area_normals);
	helper_renderVector(bAreaMethod_RenderTriNormals, c, bAreaMethod_normalizeNormals ? normal_n : normal_v, vec3(0.f, 0.f, 1.f), timestamp_area_normals);

	// The area of a triangle is 
	float areaABC = glm::dot(normal_v, glm::cross((b - a), (c - a)));
	helper_renderCrossArea(bAreaMethod_renderFullArea, (b - a), (c - a), a + /*back it up a bit*/normal_n * -0.01f, vec3(0.25f), timestamp_area_fullarea);
	renderCrossVecHelper(bAreaMethod_renderFullArea, (b - a), (c - a), a);

	float areaPBC = glm::dot(normal_v, glm::cross((b - pnt), (c - pnt)));
	helper_renderCrossArea(bAreaMethod_renderPBC_Area, (b - pnt), (c - pnt), pnt, vec3(0.5f, 0.f, 0.f), timestamp_area_PBC_area);
	renderCrossVecHelper(bAreaMethod_renderPBC_Area, (b - pnt), (c - pnt), pnt);

	float areaPCA = glm::dot(normal_v, glm::cross((c - pnt), (a - pnt)));
	helper_renderCrossArea(bAreaMethod_renderPCA_Area, (c - pnt), (a - pnt), pnt, vec3(0.0f, 0.5f, 0.f), timestamp_area_PCA_area);
	renderCrossVecHelper(bAreaMethod_renderPCA_Area, (c - pnt), (a - pnt), pnt);

	/////////////////////////////////////
	//area PAB isn't necessary, but perhaps should render it too?
	float areaPAB = glm::dot(normal_v, glm::cross((a - pnt), (b - pnt)));
	helper_renderCrossArea(bAreaMethod_renderPAB_Area, (a - pnt), (b - pnt), pnt, vec3(0.0f, 0.0f, 0.5f), timestamp_area_PAB_area);
	renderCrossVecHelper(bAreaMethod_renderPAB_Area, (a - pnt), (b - pnt), pnt);
	/////////////////////////////////////

	bary.x = areaPBC / areaABC; // alpha
	bary.y = areaPCA / areaABC; // beta
	bary.z = 1.0f - bary.x - bary.y; // gamma

}

void BarycentricsDemo::renderGame_Barycentric_SolvingLinearSystem(float dt_sec)
{

}

glm::vec3 BarycentricsDemo::projectAontoB(const glm::vec3& a, const glm::vec3& b)
{
	using namespace glm;

	//scalar projection is doting a vector onto the normalized vector of b
	//		vec3 bUnitVec = b / ||b||;	//ie normali
	//		scalarProj = a dot (b/||b||);	//ie dot with bUnitVec
	// you then then need to scale up the bUnitVector to get a projection vector
	//		vec3 projection = bUnitVector * scalarProjection
	// this can be simplified so we never have to do a square root for normalization (this looks better when written in 
	//		vec3 projection = bUnitVector * scalarProjection
	//		vec3 projection = b / ||b||	  * a dot (b/||b||)
	//		vec3 projection = b / ||b||	  * (a dot b) / ||b||		//factor around dot product
	//		vec3 projection =     b * (a dot b) / ||b||*||b||		//multiply these two terms
	//		vec3 projection =     b * ((a dot b) / ||b||*||b||)		//recale dot product will product scalar, lump scalars in parenthesis
	//		vec3 projection =     ((a dot b) / ||b||*||b||) * b;	//here b is a vector, so we have scalar * vector;
	//		vec3 projection =     ((a dot b) / (b dot b) * b;	//recall that dot(b,b) == ||b||^2 == ||b||*||b||
	vec3 projection = (glm::dot(a, b) / glm::dot(b, b)) * b;
	return projection;
}

void BarycentricsDemo::render_UI()
{
	InteractableDemoBase::render_UI();
#if WITH_IMGUI

	static bool bFirstDraw = true;
	if (bFirstDraw)
	{
		bFirstDraw = false;
		ImGui::SetNextWindowPos({ 1000, 0 });
	}

	ImGuiWindowFlags flags = 0;
	ImGui::Begin("Barycentric Coordinates Calculation Methods", nullptr, flags);
	{
		//static int baryModeProxy = 0;
		if (ImGui::RadioButton("Intuitive Method", barymode == EBarycentricMode::INTUITIVE_METHOD))
		{
			barymode = EBarycentricMode::INTUITIVE_METHOD;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Optimized Projection Method", barymode == EBarycentricMode::OPTIMIZED_PROJECTION))
		{
			barymode = EBarycentricMode::OPTIMIZED_PROJECTION;
		}
		//ImGui::SameLine();
		if (ImGui::RadioButton("Area Method", barymode == EBarycentricMode::AREA_METHOD))
		{
			barymode = EBarycentricMode::AREA_METHOD;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Linear Eq Method", barymode == EBarycentricMode::LINEAR_SYSTEMS_METHOD))
		{
			barymode = EBarycentricMode::LINEAR_SYSTEMS_METHOD;
		}

		//move to end so people don't check all these first.
		//if (ImGui::Checkbox("bRenderBarycentricA", &bRenderBarycentricA)) { bTestPointUpdated = true; } //update test point so we refresh projection anims
		//if (ImGui::Checkbox("bRenderBarycentricB", &bRenderBarycentricB)) { bTestPointUpdated = true; }
		//if (ImGui::Checkbox("bRenderBarycentricC", &bRenderBarycentricC)) { bTestPointUpdated = true; }
		ImGui::Separator();

		ImGui::Checkbox("Wireframe", &bWireframe);

		ImGui::Separator();
		if (barymode == EBarycentricMode::INTUITIVE_METHOD)
		{
			if (ImGui::Checkbox("Render B To A Vector", &bRenderBToA)) { timestamp_RenderBToA = tickedTime; }
			if (ImGui::Checkbox("Render B To C Vector", &bRenderBToC)) { timestamp_RenderBToC = tickedTime; }
			if (ImGui::Checkbox("Render BC Projection", &bRenderBCProj)) { timestamp_RenderBCProj = tickedTime; }
			if (ImGui::Checkbox("Render Perpendicular To Edge Vector", &bRender_PerpendicularToEdge)) { timestamp_Render_PerpendicularToEdge = tickedTime; }
			if (ImGui::Checkbox("Render Edge-Projection-Point To TestPoint Vector", &bRender_EdgeProjectPointToTestPoint)) { timestamp_Render_EdgeProjectPointToTestPoint = tickedTime; }
			if (ImGui::Checkbox("Render TestPoint Projection On To Perpendicular", &bRenderTestPointProjectionOntoPerpendicular)) { timestamp_RenderTestPointProjectionOntoPerpendicular = tickedTime; }
		}
		else if (barymode == EBarycentricMode::OPTIMIZED_PROJECTION)
		{
			if (ImGui::Checkbox("Render AB Vector", &bRenderAB)) { timestamp_renderbRenderAB = tickedTime; }
			if (ImGui::Checkbox("Render BC Vector", &bRenderCB)) { timestamp_renderbRenderCB = tickedTime; }
			if (ImGui::Checkbox("Render AtoTestPoint Vector", &bRender_AtoTestPnt)) { timestamp_renderbRender_AtoTestPnt = tickedTime; }
			if (ImGui::Checkbox("Render Projection To BC", &bRender_ProjToCB)) { timestamp_renderbRender_Perpendicular = tickedTime; }
			if (ImGui::Checkbox("Render VectorFromFirstProjection (Perpendicular) ", &bRender_VectorFromFirstProjection)) { timestamp_renderbRender_VectorFromFirstProjection = tickedTime; }
			ImGui::Text("(hint: move triangle A point to origin/center)");
			if (ImGui::Checkbox("Render project TestPoint On To Perpendicular", &bRender_projTestPointOntoPerpendicular)) { timestamp_renderbRender_projTestPointOntoPerpendicular = tickedTime; }
			if (ImGui::Checkbox("Render project AB On To Perpendicular", &bRender_projABontoPerpendicular)) { timestamp_renderbRender_projABontoPerpendicular = tickedTime; }
		}
		else if (barymode == EBarycentricMode::AREA_METHOD)
		{

			if (ImGui::Checkbox("Render Full Area (recommend wireframe mode)", &bAreaMethod_renderFullArea)) { timestamp_area_fullarea = tickedTime; }
			if (ImGui::Checkbox("Render PBC Area", &bAreaMethod_renderPBC_Area)) { timestamp_area_PBC_area = tickedTime; }
			if (ImGui::Checkbox("Render PCA Area", &bAreaMethod_renderPCA_Area)) { timestamp_area_PCA_area = tickedTime; }
			if (ImGui::Checkbox("Render PAB Area", &bAreaMethod_renderPAB_Area)) { timestamp_area_PAB_area = tickedTime; }
			if (ImGui::Checkbox("Render Triangle Normals", &bAreaMethod_RenderTriNormals)) { timestamp_area_normals = tickedTime; }
			if (ImGui::Checkbox("Render CrossProduct Vectors", &bRenderCrossProductVectors)) { timestamp_crossproductVecs = tickedTime; }

			ImGui::Separator();
			ImGui::Text("Visualization Options");
			ImGui::Checkbox("Normalize Normals", &bAreaMethod_normalizeNormals);
			if (ImGui::Checkbox("Render CrossVec_first", &bRenderCrossVec_first)) { timestamp_crossvecfirst = tickedTime; }
			if (ImGui::Checkbox("Render CrossVec_second ", &bRenderCrossVec_second)) { timestamp_crossvecsecond = tickedTime; }
			ImGui::Checkbox("Render Half Cross Product Area", &bRenderHalfAreas);

		}

		else if (barymode == EBarycentricMode::LINEAR_SYSTEMS_METHOD)
		{

		}


		ImGui::Separator();
		ImGui::Checkbox("Shriley Book Ground Truth", &bRenderShirleyVersion);
		ImGui::Checkbox("Real Time Collision Book Ground Truth", &bRenderRealTimeCollisionBook);

		ImGui::Separator();

		ImGui::Text("Recommend Initially Only Check A Below.");
		if (ImGui::Checkbox("Render Barycentric A Animations", &bRenderBarycentricA)) { bDraggableTestPointUpdated = true; } //update test point so we refresh projection anims
		if (ImGui::Checkbox("Render Barycentric B Animations", &bRenderBarycentricB)) { bDraggableTestPointUpdated = true; }
		if (ImGui::Checkbox("Render Barycentric C Animations", &bRenderBarycentricC)) { bDraggableTestPointUpdated = true; }

		/////////////////////////////////////////////////////////////////////////
		// Extras to help understand things related to barycentrics.
		/////////////////////////////////////////////////////////////////////////
		ImGui::Separator();
		static bool bExtras = false;
		ImGui::Checkbox("Extras", &bExtras);
		if (bExtras)
		{
			ImGui::Text("Barycentrics are NOT just point weights.");
			ImGui::Text("How to move the test point using point weights.");
			ImGui::Text("location = pointA*weightA + pointB*weightB + pointC*weightC");
			ImGui::Text("");
			ImGui::Text("Try typing some arbitrary weights to test.");
			ImGui::Text("Try typing corner barycentrics eg:(1,0,0).");
			ImGui::Text("Try typing (0.5,0.5,0).");
			ImGui::Text("Try typing an invalid barycentric. eg:(1,1,0) and (1,1,1).");
			if (ImGui::Button("Apply"))
			{
				if (testPoint && pntA && pntB && pntC)
				{
					testPoint->setPosition(
						userTypedWeightValues.x* pntA->getPosition()
						+ userTypedWeightValues.y * pntB->getPosition()
						+ userTypedWeightValues.z * pntC->getPosition()
					);
				}
				updateUserTypedWeights();	
			}
			ImGui::SameLine();
			ImGui::InputFloat3("Point Weights", &userTypedWeightValues.x);
		}
	}


	ImGui::End();
#endif //WITH_IMGUI
}

#define COMPILETIME_SQUARE(val) val * val

void BarycentricsDemo::tick(float dt_sec)
{
	using namespace glm;
	InteractableDemoBase::tick(dt_sec);

	//if test point is not within some distance to plane, move it to triangle plane
	{
		vec3 toTestPoint = testPoint->getPosition() - pntA->getPosition();

		vec3 edge1 = pntB->getPosition() - pntA->getPosition();
		vec3 edge2 = pntC->getPosition() - pntA->getPosition();
		vec3 normal = glm::cross(edge1, edge2);

		vec3 project = projectAontoB(toTestPoint, normal);
		float distFromPlane2 = glm::length2(project);
		constexpr float distTolerance2 = COMPILETIME_SQUARE(0.01f);
		if (distFromPlane2 > distTolerance2)
		{
			glm::vec3 newPos = testPoint->getPosition();
			newPos = newPos + -project; //may need to do some sign comparison here
			if (!anyValueNAN(newPos))
			{
				testPoint->setPosition(newPos);
			}
		}
	}

	//since some of these are shared between multiple triangle points, the tick happens in render since it will get updated data.
	//this means that copies of each point will animate at the same time, but that seems less-bad than making hacks.
	//projAnim_BC->tick(dt_sec);
	//projAnim_PointOnPerpendicular->tick(dt_sec);
	//projAnim_ab_onto_cb->tick(dt_sec);
	//projAnim_testPointOnPerpendicular->tick(dt_sec);
	//projAnim_aBOnPerpendicular->tick(dt_sec);

	tickedTime += dt_sec;
}


void BarycentricsDemo::gatherInteractableCubeObjects(std::vector<const TriangleList_SNO*>& objectList)
{
	InteractableDemoBase::gatherInteractableCubeObjects(objectList);

	objectList.push_back(&pntA->pointCollision->getTriangleList());
	objectList.push_back(&pntB->pointCollision->getTriangleList());
	objectList.push_back(&pntC->pointCollision->getTriangleList());

	objectList.push_back(&testPoint->pointCollision->getTriangleList());
}

void BarycentricsDemo::handleTestPointUpdated(const TutorialEngine::VisualPoint& pnt)
{
	bDraggableTestPointUpdated = true;
	updateUserTypedWeights();
}



void BarycentricsDemo::updateUserTypedWeights()
{
	if (testPoint && pntA && pntB && pntC)
	{
		realtimecollisiondetectionbook_Barycentric(testPoint->getPosition(), pntA->getPosition(), pntB->getPosition(), pntC->getPosition(),
			userTypedWeightValues.x, userTypedWeightValues.y, userTypedWeightValues.z);
	}
}

void BarycentricsDemo::helper_renderVector(bool bShouldRender, glm::vec3 start, glm::vec3 dir, glm::vec3 color)
{
	using namespace glm;
	const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
	if (rd)
	{
		if (bShouldRender)
		{
			genericVector->setStart(start);
			genericVector->setVector(dir);
			genericVector->color = color;
			genericVector->render(rd->projection_view, rd->camera ? rd->camera->getPosition() : vec3(0.f));
		}
	}
}

void BarycentricsDemo::helper_renderVector(bool bShouldRender, glm::vec3 start, glm::vec3 dir, glm::vec3 color, float timestampSecs, bool bDriftToOrigin)
{
	using namespace glm;

	const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
	if (bShouldRender && rd)
	{
		if (bDriftToOrigin)
		{
			//drift starts after animation is complete, and lasts duration of animation
			//float adjustedTimeStamp = timestampSecs - vectorAnimSecs; //pull animation back by enough time for vector to grow
			//float flippedDriftPerc = tickedTime < timestampSecs + vectorAnimSecs ? 0 : (1 - calcPerc(timestampSecs+vectorAnimSecs,tickedTime,vectorAnimSecs));
			float flippedDriftPerc = 1 - clampPerc(timestampSecs + vectorAnimSecs, tickedTime, vectorAnimSecs);
			genericVector->setStart(start * flippedDriftPerc);
		}
		else
		{
			genericVector->setStart(start);
		}
		genericVector->setVector(dir * clampPerc(timestampSecs, tickedTime, vectorAnimSecs));
		genericVector->color = color;
		genericVector->render(rd->projection_view, rd->camera ? rd->camera->getPosition() : glm::vec3(0.f));
	}
}

void BarycentricsDemo::helper_renderProjection(bool bShouldRender, TutorialEngine::VectorProjectionAnimation& projAnim, glm::vec3 aVec, glm::vec3 bVec, glm::vec3 aStart, glm::vec3 bStart, float dt_sec, std::optional<glm::vec3> color)
{
	using namespace glm;

	const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
	if (rd)
	{
		if (bShouldRender)
		{
			//since projAnim is shared between all 3 triangle points, we need to update this projection everytime we render.
			//but we will keep the animation progress so that they always appear in the correct location.
			projAnim.projectFromAtoB(aVec, bVec, aStart, bStart, /*reset*/false);	//only reset anim if test point wasn't updated
			projAnim.setColor(color.has_value() ? *color : vec3(1.f, 1.f, 0.f));	//set the animation to the color associated with the vector, since it is shared.
			projAnim.tick(dt_sec);													//since this is shared, we need to tick here so positions end up in the correct place.
			projAnim.setShouldRender(true); //the bool to this function will control whether or not the projection gets rendered.
			projAnim.render(rd->projection_view, rd->camera ? rd->camera->getPosition() : glm::vec3(0.f)); //prevent flickering as it hasn't been ticked yet
		}
		else
		{
			//reset animation since rendering stopped.
			//note: since this is shared for each point in the triangle, this means all projections of a given bool will animate at same time.
			projAnim.projectFromAtoB(aVec, bVec, aStart, bStart, /*reset*/true);
			projAnim.setShouldRender(false);
		}
	}
}

void BarycentricsDemo::helper_renderCrossArea(bool bShouldRender, glm::vec3 first, glm::vec3 second, glm::vec3 start, glm::vec3 color, float timestamp_start)
{
	const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
	if (rd)
	{
		if (bShouldRender)
		{
			float perc = clampPerc(timestamp_start, tickedTime, vectorAnimSecs);

			float fistHalfPerc = glm::clamp(perc, 0.f, 0.5f) / 0.5f;
			float secondHalfPerc = glm::clamp(perc - 0.5f, 0.f, 0.5f) / 0.5f;

			triRender->renderTriangle(start, start + first * fistHalfPerc, start + second * fistHalfPerc, color, rd->projection_view);

			//second triangle
			glm::vec3 second_StartA = start + first;
			glm::vec3 second_StartB = start + second;

			if (!bRenderHalfAreas)
			{
				glm::vec3 second_End = second_StartA + second;
				glm::vec3 proj_ontoAB = projectAontoB(second_End - second_StartA, second_StartB - second_StartA); //similar to projection method of barycentrics, find a perpendicular to a point on a triangle -- then we're sliding up along that perpendcular
				glm::vec3 perpStartPoint = second_StartA + proj_ontoAB;
				glm::vec3 perpendicular_v = second_End - perpStartPoint;

				triRender->renderTriangle(second_StartA, second_StartB, perpStartPoint + secondHalfPerc * perpendicular_v, color, rd->projection_view);
			}

			glm::vec3 crossResult = glm::cross(first, second);
			helper_renderVector(bRenderCrossProductVectors, start, crossResult, color);

		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	std::cout << "Barycentric Coordinate Started" << std::endl;

	sp<TutorialUtils::TutorialEngine> engine = new_sp<TutorialUtils::TutorialEngine>();
	engine->start([&]() {
		static sp<SceneNode> applicationLifetimeObject = new_sp<BarycentricsDemo>();
		engine->addToRoot(applicationLifetimeObject);
		});
}
