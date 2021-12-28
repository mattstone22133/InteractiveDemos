#include "Framework/InteractableDemoBase.h"
#include "Utils/TutorialRayUtils.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "GeometryData/TriangleCube.h"

//#todo #port #imgui
//#define  WITH_IMGUI 1
#if WITH_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "EngineSystems/EditorUISystem/EditorUISystem.h"
#endif // WITH_IMGUI

#include <cstdint>
#include "SceneNode.h"
#include "EngineSystems/RenderSystem/RenderSystem.h"
#include "EngineSystems/TimeSystem/TickGroupManager.h"
#include "EngineSystems/WindowSystem/WindowSystem.h"
#include "EngineSystems/WindowSystem/WindowWrapper.h"
#include "Engine.h"

using namespace Engine;
namespace TutorialEngine
{

	void SceneNode_TriangleList::setLocalPosition(const glm::vec3& pos)
	{
		SceneNode::setLocalPosition(pos);
		transformTriangleList(getWorldMat());
	}

	void SceneNode_TriangleList::setLocalRotation(const glm::quat newLocalRotQuat)
	{
		SceneNode::setLocalRotation(newLocalRotQuat);
		transformTriangleList(getWorldMat());
	}

	void SceneNode_TriangleList::setLocalScale(const glm::vec3& newScale)
	{
		SceneNode::setLocalScale(newScale);
		transformTriangleList(getWorldMat());
	}

	void SceneNode_TriangleList::setLocalTransform(const Engine::Transform& newTransform)
	{
		SceneNode::setLocalTransform(newTransform);
		transformTriangleList(getWorldMat());
	}

	void SceneNode_TriangleList::v_CleanComplete()
	{
		SceneNode::v_CleanComplete();
		transformTriangleList(getWorldMat());
	}

	const TriangleList_SNO& SceneNode_TriangleList::getTriangleList()
	{
		if (isDirty())
		{
			requestClean();

			//now that clean has happenedd, update the triangle before returning
			transformTriangleList(getWorldMat());
		}
		return *myTriangleList;
	}

	void SceneNode_TriangleList::transformTriangleList(const glm::mat4& worldMat)
	{
		myTriangleList->transform(worldMat);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	//
	//
	//
	//
	//InteractableDemo
	// 
	//
	//
	//
	//
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	InteractableDemoBase::InteractableDemoBase()
	{
		++instanceCount;
		selectMouseButton = GLFW_MOUSE_BUTTON_RIGHT;
	}

	InteractableDemoBase::~InteractableDemoBase()
	{
		--instanceCount;

		if (instanceCount == 0)
		{
			debugCubeRenderer = nullptr;
			lineRenderer = nullptr;
			planeRenderer = nullptr;
		}
	}

	void InteractableDemoBase::postConstruct()
	{
		SceneNode::postConstruct();

		if (bInitOnPostConstruct)
		{
			init();
		}
		if (bRegisterEditorUiOnPostConstruct)
		{
#if WITH_IMGUI
			UISystem_Editor::get().onUIFrameStarted.addWeakObj(sp_this(), &InteractableDemoBase::render_UI);
#endif //WITH_IMGUI
		}

		Engine::EngineBase::get().getGameTimeManager().getEvent(TickGroups::get().GAME).addWeakObj(sp_this(), &InteractableDemoBase::tick);
	}

	void InteractableDemoBase::render_game(float /*dt_sec*/)
	{
		using namespace glm;

		const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();

		if (rd)
		{
			if (bDrawDebugCubes)
			{
				std::vector<const TriangleList_SNO*> objectList;
				gatherInteractableCubeObjects(objectList);

				for (const TriangleList_SNO* triList_SNO : objectList)
				{
					if (triList_SNO->owner)
					{
						debugCubeRenderer->render(rd->projection_view, triList_SNO->owner->getWorldMat());
					}
				}
			}

			if (bDebugLastRay && previousRayCast.has_value())
			{
				lineRenderer->renderLine(previousRayCast->start, previousRayCast->start + (previousRayCast->dir * 10.f), glm::vec3(1, 0, 0), rd->projection_view);
			}
			if (bRenderLineGeneration && start_linePnt && end_linePnt)
			{
				lineRenderer->renderLine(start_linePnt->getLocalPosition(), end_linePnt->getLocalPosition(), glm::vec3(1, 0, 0), rd->projection_view);
			}
			if (bDrawInteractionPlane)
			{
				vec3 camPos = rd->camera->getPosition();
				vec3 camFront = rd->camera->getFront();
				vec3 planePnt = camPos + camFront * lineCreationDistFromCamera;

				vec3 scale{ lineCreationDistFromCamera * 4 }; //this is a rather arbitrary value that is somewhat related to the camera

				planeRenderer->renderPlane(planePnt, -camFront, scale, vec4(vec3(0.1f), 1), rd->projection_view);
			}
		}
	}

	void InteractableDemoBase::render_UI()
	{
#if WITH_IMGUI
		//#todo #port #imgui
		//#todo not sure tehse should exist in the base intractable demo
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGuiWindowFlags flags = 0;
		ImGui::Begin("Interactable Demo Debug Window", nullptr, flags);
		if (bEnableDebugUI)
		{
			ImGui::Checkbox("draw debug cubes", &bDrawDebugCubes);
			ImGui::Checkbox("debug ray cast", &bDebugLastRay);
			ImGui::Checkbox("draw interaction plane", &bDrawInteractionPlane);
		}
		ImGui::End();
#endif // WITH_IMGUI
	}

	void InteractableDemoBase::init()
	{
		nodePool.push_back(new_sp<Engine::SceneNode>());
		nodePool.push_back(new_sp<Engine::SceneNode>());

		//same shape is used for all clickable collision, so we can just pick one of the vector's ends.
		debugCubeRenderer = debugCubeRenderer ? debugCubeRenderer : new_sp<TutorialEngine::TriangleListDebugger>(TutorialEngine::TriangleCube{}.triangles);

		lineRenderer = lineRenderer ? lineRenderer : new_sp<TutorialEngine::LineRenderer>();

		planeRenderer = planeRenderer ? planeRenderer : new_sp<TutorialEngine::PlaneRenderer>();
		planeRenderer->bScreenDoorEffect = true;

		instanceCount++;

		RenderSystem::get().onRenderDispatch.addWeakObj(sp_this(), &InteractableDemoBase::render_game);

		//TEST INPUT DELAY
		if (const sp<Window>& primaryWindow = WindowSystem::get().getPrimaryWindow())
		{
			primaryWindow->cursorPosEvent.addWeakObj(sp_this(), &InteractableDemoBase::handleCursorPositionUpdated);
		}
	}

	void InteractableDemoBase::inputPoll(float dt_sec)
	{
		const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
		if (rd && rd->camera && rd->window && rd->window.get())
		{
			GLFWwindow* glfwWindow = rd->window->get();
			bool bCTRL = glfwGetKey(glfwWindow, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS || glfwGetKey(glfwWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
			bool bALT = glfwGetKey(glfwWindow, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS || glfwGetKey(glfwWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
			bool bSHIFT = glfwGetKey(glfwWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

			if (glfwGetMouseButton(glfwWindow, selectMouseButton) == GLFW_PRESS)
			{
				if (!bSelectButtonPressed) //must be in a separate branch from above to prevent releasing hold
				{
					bSelectButtonPressed = true;
					if (glfwGetInputMode(glfwWindow, GLFW_CURSOR))
					{
						CameraRayCastData_Triangles raycastQuery;
						raycastQuery.camFront_n = rd->camera->getFront();
						raycastQuery.camRight_n = rd->camera->getRight();
						raycastQuery.camUp_n = rd->camera->getUp();
						raycastQuery.camPos = rd->camera->getPosition();
						raycastQuery.fovY_deg = glm::degrees<float>(rd->camera->getFOVy_rad());
						raycastQuery.window = glfwWindow;

						//TODO revisit having a static array... that is bad. but fixing up compile issues during port on linux and dont want to reevaluate this right now.
						static std::vector<const TriangleList_SNO*> typedObjects = []() {
							std::vector<const TriangleList_SNO*> tempTypedObjects;
							tempTypedObjects.reserve(20);
							return tempTypedObjects; 
						}();
						typedObjects.clear();

						//gather objects from subclass in type safe way then convert them into form raycast can work with. This means casting will be safe.
						gatherInteractableCubeObjects(typedObjects);
						raycastQuery.objectList.reserve(typedObjects.size());
						for (const TriangleList_SNO* typedTriList : typedObjects)
						{
							raycastQuery.objectList.push_back(typedTriList);
						}

						std::optional<TriangleObjectRaycastResult> raycastResult = rayCast_TriangleObjects(raycastQuery);
						if (raycastResult)
						{
							//fast safe cast because we gathered objets in a typed manner.
							activeClickTarget = static_cast<const TriangleList_SNO*>(raycastResult->hitObject);
							previousRayCast = raycastResult->castRay;
						}
						else
						{
							// DRAW LINE BETWEEN POINTS
							previousRayCast = rayCast(raycastQuery);
							if (previousRayCast)
							{
								glm::vec3 camFront = rd->camera->getFront();
								if (std::optional<glm::vec3> startPnt = RayTests::rayPlaneIntersection(*previousRayCast, -camFront, rd->camera->getPosition() + camFront * lineCreationDistFromCamera))
								{
									glm::vec3 endPnt = *startPnt;

									if (!bALT)
									{
										//draw vectors when not holding alt
										assert(nodePool.size() >= 2);
										start_linePnt = nodePool[0].get();
										end_linePnt = nodePool[1].get();

										start_linePnt->setLocalPosition(*startPnt);
										end_linePnt->setLocalPosition(endPnt);
									}
									else
									{
										assert(nodePool.size() >= 1);
										customPoint = nodePool[0].get();
										customPoint->setLocalPosition(*startPnt);
									}
								}
							}
						}

						glfwGetCursorPos(glfwWindow, &lastMousePos.x, &lastMousePos.y);
					}
				}
			}
			else
			{
				activeClickTarget = nullptr;
				start_linePnt = nullptr;
				end_linePnt = nullptr;
				customPoint = nullptr;
				bSelectButtonPressed = false;
			}


			if (glfwGetKey(glfwWindow, GLFW_KEY_O) == GLFW_PRESS)
			{
				lineCreationDistFromCamera += dt_sec * lineCreationDistAdjustSpeedSec * (bSHIFT ? 0.1f : 1.f) * (bCTRL ? 10.f : 1.f);
			}
			else if (glfwGetKey(glfwWindow, GLFW_KEY_I) == GLFW_PRESS)
			{
				lineCreationDistFromCamera -= dt_sec * lineCreationDistAdjustSpeedSec * (bSHIFT ? 0.1f : 1.f) * (bCTRL ? 10.f : 1.f);
			}

		}
	}

	void InteractableDemoBase::tick(float dt_sec)
	{
		using namespace glm;
		using std::optional;

		inputPoll(dt_sec);

		const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
		if (rd && rd->camera && rd->window && rd->window->get())
		{
			GLFWwindow* glfwWindow = rd->window->get();
			//this will be useful for creating vectors... so not deleting just yet

			Engine::SceneNode* targetNode = nullptr;

			if (activeClickTarget && activeClickTarget->owner)
			{
				//manipulate some scene node in the demo
				targetNode = activeClickTarget->owner;
			}
			else if (start_linePnt && end_linePnt)
			{
				//create a line by dragging mouse
				targetNode = end_linePnt;
			}
			else if (customPoint)
			{
				targetNode = customPoint;
			}

			if (targetNode)
			{
				vec3 cameraPosition = rd->camera->getPosition();
				vec3 cameraFront = rd->camera->getFront();

				CameraRayCastData raycastQuery;
				raycastQuery.camFront_n = cameraFront;
				raycastQuery.camRight_n = rd->camera->getRight();
				raycastQuery.camUp_n = rd->camera->getUp();
				raycastQuery.camPos = cameraPosition;
				raycastQuery.fovY_deg = glm::degrees<float>(rd->camera->getFOVy_rad());
				raycastQuery.window = glfwWindow;
				if (optional<Ray> ray = rayCast(raycastQuery))
				{
					vec3 pointOnPlane = targetNode->getWorldPosition();
					const vec3 planeNormal = -cameraFront;

					if (optional<vec3> newPoint = RayTests::rayPlaneIntersection(*ray, planeNormal, pointOnPlane))
					{
						const sp<Engine::SceneNode>& parent = targetNode->getParent();
						glm::mat4 localFromWorld = parent ? parent->getInverseWorldMat() : mat4(1.f);

						targetNode->setLocalPosition(localFromWorld * glm::vec4(*newPoint, 1.f));
					}
					previousRayCast = ray;
				}
			}

			//MOVE USING CAMERA BASIS
			//if (activeClickTarget)
			//{
			//	glm::dvec2 newMouse;
			//	glfwGetCursorPos(glfwWindow, &newMouse.x, &newMouse.y);

			//	glm::dvec2 mouseDelta = newMouse - lastMousePos;
			//	lastMousePos = newMouse; //record last mouse position now that we have created the delta

			//	glm::vec4 moveDelta_ws = glm::vec4(rd->camera->getUp() * float(mouseDelta.y) + rd->camera->getRight() * float(mouseDelta.x), 0);

			//	if (activeClickTarget->owner)
			//	{
			//		SceneNode_TriangleList* node = activeClickTarget->owner;
			//		glm::mat4 localFromWorld = node->getInverseWorldMat();

			//		//convert the world vector into a vector of the object's local space so that translation looks correct with parent node 
			//		//note that I am not set up to test this at time of writing, hopefully it works but expect it to break :) -- #TODO please remove this comment after testing
			//		glm::vec3 moveDelta_ls = glm::vec3(localFromWorld * moveDelta_ws);
			//		node->setLocalPosition(node->getLocalPosition() + moveDelta_ls);
			//	}
			//	else
			//	{
			//		std::cerr << "attempting to move an triangle list object that has no owning scene node" << std::endl;
			//	}
			//}
		}
	}

	std::optional<glm::vec3> InteractableDemoBase::getDrawLineStart()
	{
		if (start_linePnt)
		{
			return start_linePnt->getLocalPosition();
		}

		return std::nullopt;
	}

	std::optional<glm::vec3> InteractableDemoBase::getDrawLineEnd()
	{
		if (end_linePnt)
		{
			return end_linePnt->getLocalPosition();
		}

		return std::nullopt;
	}

	std::optional<glm::vec3> InteractableDemoBase::getDrawPoint()
	{
		if (customPoint)
		{
			return customPoint->getLocalPosition();
		}
		return std::nullopt;
	}

	void InteractableDemoBase::gatherInteractableCubeObjects(std::vector<const TriangleList_SNO*>& /*objectList*/)
	{
		//default do nothing so base classes can call super
	}

	void InteractableDemoBase::handleCursorPositionUpdated(double /*x*/, double /*y*/)
	{
		//TODO properly hook up mouse updates; but this doesn't seem to have an effect on percieved mouse lag for barycentric demo, so not properly hooking up at the moment.
		//float dt_sec = Engine::EngineBase::get().getGameTimeManager().getDeltaTimeSecs();
		//inputPoll(dt_sec);
	}


	/*static*/ sp<TutorialEngine::TriangleListDebugger> InteractableDemoBase::debugCubeRenderer = nullptr;
	/*static*/ sp<TutorialEngine::LineRenderer> InteractableDemoBase::lineRenderer = nullptr;
	/*static*/ sp<TutorialEngine::PlaneRenderer> InteractableDemoBase::planeRenderer = nullptr;
	/*static*/ int32_t InteractableDemoBase::instanceCount = 0;


}
