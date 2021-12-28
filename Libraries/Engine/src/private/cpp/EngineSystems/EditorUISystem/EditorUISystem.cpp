#include "EngineSystems/EditorUISystem/EditorUISystem.h"

#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "Engine.h"
#include "EngineSystems/WindowSystem/WindowSystem.h"
#include "EngineSystems/WindowSystem/WindowWrapper.h"

#if WITH_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif //WITH_IMGUI

namespace Engine
{
	void UISystem_Editor::initSystem()
	{
#if WITH_IMGUI
		//requires window system be available; which is safe within the initSystem 
		WindowSystem& windowSystem = WindowSystem::get();
		windowSystem.onWindowLosingOpenglContext.addWeakObj(sp_this(), &UISystem_Editor::handleLosingOpenGLContext);
		windowSystem.onWindowAcquiredOpenglContext.addWeakObj(sp_this(), &UISystem_Editor::handleWindowAcquiredOpenGLContext);

		//in case things are refactored and windows are created during system initialization, this will catch
		//the edge case where a window is already created before we start listening to the primary changed delegate
		if (const sp<Window>& window = windowSystem.getPrimaryWindow())
		{
			handleWindowAcquiredOpenGLContext(window);
		}

		EngineBase::get().onRenderDispatchEnded.addWeakObj(sp_this(), &UISystem_Editor::handleRenderDispatchEnding);
#endif //WITH_IMGUI
	}

#if WITH_IMGUI
	void UISystem_Editor::handleLosingOpenGLContext(const sp<Window>& window)
	{
		if (!imguiBoundWindow.expired())
		{
			//assuming window == imguiBoundWindow since it ImGui should always be associated with current bound context
			if (window)
			{
				window->onRawGLFWKeyCallback.removeStrong(sp_this(), &UISystem_Editor::handleRawGLFWKeyCallback);
				window->onRawGLFWCharCallback.removeStrong(sp_this(), &UISystem_Editor::handleRawGLFWCharCallback);
				window->onRawGLFWMouseButtonCallback.removeStrong(sp_this(), &UISystem_Editor::handleRawGLFWMouseButtonCallback);
				window->onRawGLFWScrollCallback.removeStrong(sp_this(), &UISystem_Editor::handleRawGLFWScroll);
				destroyImGuiContext();
			}
		}
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::handleWindowAcquiredOpenGLContext(const sp<Window>& window)
	{
		//make sure we have cleaned up the old context and have nullptr within the imguiBoundWindow
		assert(imguiBoundWindow.expired());

		if (window)
		{
			//set up IMGUI
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO imguiIO = ImGui::GetIO();
			ImGui::StyleColorsDark();
			//ImGui_ImplOpenGL3_Init("#version 330 core");							 //seems to be window independent, but example code has this set after window
			ImGui_ImplOpenGL3_Init("#version 100");							 //seems to be window independent, but example code has this set after window
			ImGui_ImplGlfw_InitForOpenGL(window->get(), /*install callbacks*/false); //false will require manually calling callbacks in our own handlers
			imguiBoundWindow = window;

			//manually unregister these when window loses active context
			window->onRawGLFWKeyCallback.addStrongObj(sp_this(), &UISystem_Editor::handleRawGLFWKeyCallback);
			window->onRawGLFWCharCallback.addStrongObj(sp_this(), &UISystem_Editor::handleRawGLFWCharCallback);
			window->onRawGLFWMouseButtonCallback.addStrongObj(sp_this(), &UISystem_Editor::handleRawGLFWMouseButtonCallback);
			window->onRawGLFWScrollCallback.addStrongObj(sp_this(), &UISystem_Editor::handleRawGLFWScroll);
		}
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::handleRawGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (!imguiBoundWindow.expired())
		{
			ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
		}
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::handleRawGLFWCharCallback(GLFWwindow* window, unsigned int c)
	{
		if (!imguiBoundWindow.expired())
		{
			ImGui_ImplGlfw_CharCallback(window, c);
		}
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::handleRawGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (!imguiBoundWindow.expired())
		{
			ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
		}
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::handleRawGLFWScroll(GLFWwindow* window, double xOffset, double yOffset)
	{
		if (!imguiBoundWindow.expired())
		{
			ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
		}
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::handleRenderDispatchEnding(float /*dt_sec*/)
	{
		processUIFrame();
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::processUIFrame()
	{
		const bool bHasUiToRender = onUIFrameStarted.numBound() > 0;
		if (bHasUiToRender)
		{
			if (!imguiBoundWindow.expired() && bUIEnabled)
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				onUIFrameStarted.broadcast();
				//UI elements will draw during first broadcast

				ImGui::EndFrame();
				onUIFrameEnded.broadcast();
			}

			if (!imguiBoundWindow.expired() && bUIEnabled)
			{
				//UI will have set up widgets between frame
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
		}
	}
#endif //WITH_IMGUI

#if WITH_IMGUI
	void UISystem_Editor::destroyImGuiContext()
	{
		//shut down IMGUI
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
		imguiBoundWindow = sp<Window>(nullptr);
	}
#endif //WITH_IMGUI

	void UISystem_Editor::shutdown()
	{
#if WITH_IMGUI
		if (!imguiBoundWindow.expired())
		{
			destroyImGuiContext();
		}
#endif //WITH_IMGUI
	}


}

