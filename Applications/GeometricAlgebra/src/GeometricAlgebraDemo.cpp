#include <iostream> //todo -- remove this if it isn't used!

#include <TutorialEngine.h>
#include <GameObjectBase.h>
#include "GeometricAlgebraDemo.h"

#if WITH_IMGUI
	#include "imgui.h"
	#include "imgui_impl_glfw.h"
	#include "imgui_impl_opengl3.h"
#endif //WITH_IMGUI
#if WITH_KLEIN
	#include "klein/klein.hpp"
#endif //WITH_KLEIN

using namespace Engine;
using namespace TutorialEngine;
using namespace MathUtils;

#if HTML_BUILD
#include <wasm_simd128.h>
#endif //HTML_BUILD


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	//DELETE ME TEST
#if WITH_KLEIN
	kln::point pointA(1.f, 3.f, 3.f);
	kln::point pointB(3.f, 3.f, 3.f);
	kln::point pointC(3.f, 1.f, 3.f);
	//kln::point pointA(3.f, 3.f, 3.f);
	//kln::point pointB(3.f, 3.f, 1.f);
	//kln::point pointC(3.f, 1.f, 1.f);
	kln::plane plane = pointA & pointB & pointC;

	plane.normalize();
	glm::vec3 planeNormal(plane.x(), plane.y(), plane.z());
	std::cout << "plane normal:" << planeNormal.x << " " << planeNormal.y << " " << planeNormal.z << std::endl;

	//How can we get points on a plane efficiently with this implementation of a plane?

	kln::dual dualResult = pointA & plane;
#endif //WITH_KLEIN
	//DELETE ME TEST OVER

	std::cout << "Geometric Algebra Demo Started" << std::endl;

	sp<TutorialUtils::TutorialEngine> engine = new_sp<TutorialUtils::TutorialEngine>();
	engine->start([&]() {
		static sp<SceneNode> applicationLifetimeObject = new_sp<GeometricAlgebraDemo>();
		engine->addToRoot(applicationLifetimeObject);
		});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Application
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GeometricAlgebraDemo::init()
{
	InteractableDemoBase::init();
}

void GeometricAlgebraDemo::render_game(float dt_sec)
{
	InteractableDemoBase::render_game(dt_sec);
	using namespace glm;

	const sp<FrameRenderData>& rd = RenderSystem::get().getFrameRenderData();
	if (rd)
	{

	}
}

void GeometricAlgebraDemo::render_UI()
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
	ImGui::Begin("Geometric Algebra Demo", nullptr, flags);
	{
		static bool bTemporaryTestBool;
		ImGui::Checkbox("Temporary", &bTemporaryTestBool);

	}

	ImGui::End();
#endif //WITH_IMGUI
}

void GeometricAlgebraDemo::tick(float dt_sec)
{
	using namespace glm;
	InteractableDemoBase::tick(dt_sec);
}

void GeometricAlgebraDemo::gatherInteractableCubeObjects(std::vector<const TriangleList_SNO*>& objectList)
{
	InteractableDemoBase::gatherInteractableCubeObjects(objectList);
}
