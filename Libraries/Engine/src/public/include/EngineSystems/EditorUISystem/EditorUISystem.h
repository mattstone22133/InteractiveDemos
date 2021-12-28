#pragma once
#include "EngineSystems/SystemBase.h"
#include "Event.h"

struct GLFWwindow;

namespace Engine
{
	class Window;

	/** UI system fpr the developer menus; separate from in game UI. */
	class UISystem_Editor : public SystemBase
		, public IEventSubscriber
	{
		IMPLEMENT_PUBLIC_STATIC_GET(UISystem_Editor, SystemBase);

	public:
		Event<> onUIFrameStarted;
		Event<> onUIFrameEnded;

		inline void setEditorUIEnabled(bool bEnable) { bUIEnabled = bEnable; }
		inline bool getEditorUIEnabled() { return bUIEnabled; }

	private:
		virtual void tick(float /*deltaSec*/) {}
		virtual void initSystem() override;
		virtual void shutdown() override;

#if WITH_IMGUI
		void handleLosingOpenGLContext(const sp<Window>& window);
		void handleWindowAcquiredOpenGLContext(const sp<Window>& window);
		void handleRawGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void handleRawGLFWCharCallback(GLFWwindow* window, unsigned int c);
		void handleRawGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void handleRawGLFWScroll(GLFWwindow* window, double xOffset, double yOffset);
		void destroyImGuiContext();
		void processUIFrame();
		void handleRenderDispatchEnding(float dt_sec);
#endif //WITH_IMGUI

	private:
		wp<Window> imguiBoundWindow;
		bool bUIEnabled = true;
	};
}