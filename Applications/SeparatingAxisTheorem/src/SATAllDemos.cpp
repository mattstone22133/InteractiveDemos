///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////
#ifdef WITH_SAT_DEMO_BUILD

#include<iostream>

#include <Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>
//#include<glad/glad.h> //include opengl headers, so should be before anything that uses those headers (such as GLFW)
//#include<GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <tuple>
#include <array>
#include <string>
#include <cmath>

#include "Utils/Shader.h"
#include "Framework/Cameras/QuaternionCamera.h"
//#include "../../GettingStarted/Camera/CameraFPS.h"
//#include "../../../InputTracker.h"
//#include "../../nu_utils.h"
//#include "../../../Shader.h"
#include "EngineSystems/WindowSystem/WindowWrapper.h"
#include "EngineSystems/WindowSystem/WindowSystem.h"
#include "TutorialEngine.h"

#include "SATComponent.h"
#include "SATUnitTestUtils.h"
#include "SATRenderDebugUtils.h"
#include "ModelLoader/SATModel.h"
#include "SATDemoInterface.h"
#include "PortedOldOpenGL3/Deprecated_InputTracker.h"
#include "SATAllDemos.h"

#ifdef WITH_SAT_DEMO_MODEL_FILES
std::shared_ptr<ISATDemo> factory_ModelDemo(int width, int height);
#endif //WITH_SAT_DEMO_MODEL_FILES
std::shared_ptr<ISATDemo> factory_DynamicGeneratedPolyDemo(int width, int height);
std::shared_ptr<ISATDemo> factory_CapsuleShape(int width, int height);
std::shared_ptr<ISATDemo> factory_CubeShape(int width, int height);
std::shared_ptr<ISATDemo> factory_Demo2D(int width, int height);

namespace
{
	//void true_main()
	//{
	//	void true_main()
	//	{
	//		using glm::vec2;
	//		using glm::vec3;
	//		using glm::mat4;

	//		int width = 1200;
	//		int height = 800;

	//		GLFWwindow* window = init_window(width, height);

	//		glViewport(0, 0, width, height);
	//		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {  glViewport(0, 0, width, height); });
	//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//		std::shared_ptr<ISATDemo> sat2DDemo = factory_Demo2D(width, height);
	//		std::shared_ptr<ISATDemo> cubeShapeDemo = factory_CubeShape(width, height);
	//		std::shared_ptr<ISATDemo> capsuleShapeDemo = factory_CapsuleShape(width, height);
	//		std::shared_ptr<ISATDemo> dynCapsuleDemo = factory_DynamicGeneratedPolyDemo(width, height);
	//		std::shared_ptr<ISATDemo> modelDemo = factory_ModelDemo(width, height);
	//		std::vector<std::shared_ptr<ISATDemo>> demos = { sat2DDemo, cubeShapeDemo, capsuleShapeDemo, dynCapsuleDemo, modelDemo };

	//		std::shared_ptr<ISATDemo> activeDemo = sat2DDemo;
	//		activeDemo->handleModuleFocused(window);

	//		InputTracker input;

	//		std::cout << "To switch between Demos, hold left control and press the number keys (not numpad) \n\n\n" << std::endl;

	//		/////////////////////////////////////////////////////////////////////
	//		// Game Loop
	//		/////////////////////////////////////////////////////////////////////
	//		while (!glfwWindowShouldClose(window))
	//		{
	//			input.updateState(window);
	//			if (input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL))
	//			{
	//				for (unsigned int key = GLFW_KEY_0; key < GLFW_KEY_9 + 1; key++)
	//				{
	//					if (input.isKeyJustPressed(window, key))
	//					{
	//						unsigned int idx = key - GLFW_KEY_0;
	//						idx -= 1; //convert to 0 based
	//						idx = idx >= demos.size() ? demos.size() - 1 : idx;

	//						activeDemo = demos[idx];
	//						activeDemo->handleModuleFocused(window);
	//						break;
	//					}
	//				}
	//			}

	//			activeDemo->tickGameLoop(window);

	//			glfwSwapBuffers(window);
	//			glfwPollEvents();
	//		}

	//		glfwTerminate();
	//	}
	//}
}

namespace SAT
{
	void SATDemoApplication::init()
	{
		Super::init();

		Engine::EngineBase::get().onShutdownInitiated.addWeakObj(sp_this(), &SATDemoApplication::handleEngineShuttingDown);

		using glm::vec2;
		using glm::vec3;
		using glm::mat4;

		int width = 1200;
		int height = 800;

		//GLFWwindow* window = init_window(width, height);
		//glViewport(0, 0, width, height);

		const sp<Engine::Window>& primaryWindow = Engine::WindowSystem::get().getPrimaryWindow();
		if (GLFWwindow* window = primaryWindow ? primaryWindow->get() : nullptr)
		{
			//TODO: enable this for flying mouse camera!
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //this should probably be handled in the engine

			std::shared_ptr<ISATDemo> sat2DDemo = factory_Demo2D(width, height);
			std::shared_ptr<ISATDemo> cubeShapeDemo = factory_CubeShape(width, height);
			std::shared_ptr<ISATDemo> capsuleShapeDemo = factory_CapsuleShape(width, height);
			std::shared_ptr<ISATDemo> dynCapsuleDemo = factory_DynamicGeneratedPolyDemo(width, height);
			demos = { sat2DDemo, cubeShapeDemo, capsuleShapeDemo, dynCapsuleDemo };

	#ifdef WITH_SAT_DEMO_MODEL_FILES
			std::shared_ptr<ISATDemo> modelDemo = factory_ModelDemo(width, height);
			demos.push_back(modelDemo);
	#endif //WITH_SAT_DEMO_MODEL_FILES

			activeDemo = sat2DDemo;
			//activeDemo = cubeShapeDemo;
			activeDemo->handleModuleFocused(window);

			std::cout << "To switch between Demos, hold left control and press the number keys (not numpad) \n\n\n" << std::endl;
		}
		else
		{
			std::cerr << "failed to get window." << std::endl;
		}
	}

	void SATDemoApplication::render_game(float dt_sec)
	{
		Super::render_game(dt_sec);

		if (!activeDemo)
		{
			//likely shutdown started and we cleared the active demo.
			return;
		}

		/////////////////////////////////////////////////////////////////////
		// Game Loop
		/////////////////////////////////////////////////////////////////////
		const sp<Engine::Window>& primaryWindow = Engine::WindowSystem::get().getPrimaryWindow();
		if (GLFWwindow* window = primaryWindow ? primaryWindow->get() : nullptr)
		{
			input.updateState(window);
			if (input.isKeyDown(window, GLFW_KEY_LEFT_CONTROL))
			{
				for (unsigned int key = GLFW_KEY_0; key < GLFW_KEY_9 + 1; ++key)
				{
					if (input.isKeyJustPressed(window, key))
					{
						unsigned int idx = key - GLFW_KEY_0;
						idx -= 1; //convert to 0 based
						idx = idx >= demos.size() ? demos.size() - 1 : idx;

						activeDemo = demos[idx];
						activeDemo->handleModuleFocused(window);
						break;
					}
				}
			}

			activeDemo->tickGameLoop(window);
		}
	}

	void SATDemoApplication::render_UI()
	{
		Super::render_UI();

	}

	void SATDemoApplication::inputPoll(float dt_sec)
	{
		Super::inputPoll(dt_sec);

	}

	void SATDemoApplication::tick(float dt_sec)
	{
		Super::tick(dt_sec);

	}

	void SATDemoApplication::handleEngineShuttingDown()
	{
		//clear out the demos so all the opengl resources get deleted before we lose opengl context.
		demos.clear();
		activeDemo = nullptr;
	}

}


#endif //WITH_SAT_DEMO_BUILD

int main()
{
#ifdef WITH_SAT_DEMO_BUILD
	std::cout << "SAT Demo Started" << std::endl;

	using namespace Engine;

	sp<TutorialUtils::TutorialEngine> engine = new_sp<TutorialUtils::TutorialEngine>();

	engine->start([&]() 
	{
		if (const sp<Window>& primaryWindow = WindowSystem::get().getPrimaryWindow())
		{
			const int width = 1200;
			const int height = 800;
			const bool bUpdateViewport = true;
			primaryWindow->setWindowSize(width, height, bUpdateViewport);
		}

		static sp<SceneNode> applicationLifetimeObject = new_sp<SAT::SATDemoApplication>();
		engine->addToRoot(applicationLifetimeObject);
	});

#endif //WITH_SAT_DEMO_BUILD
}
