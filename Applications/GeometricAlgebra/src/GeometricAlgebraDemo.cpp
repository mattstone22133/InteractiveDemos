#include <iostream>
#include <TutorialEngine.h>
#include <GameObjectBase.h>


#if WITH_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif //WITH_IMGUI

#include "GeometricAlgebraDemo.h"

#if WITH_KLEIN
#include "klein/point.hpp"
#include "klein/plane.hpp"
#endif //WITH_KLEIN

using namespace Engine;
using namespace TutorialEngine;
using namespace MathUtils;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	//DELETE ME TEST
#if WITH_KLEIN
	kln::point test(1.f, 2.f, 3.f);
	kln::plane planeTest;
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
