
#include "EngineSystems/WindowSystem/WindowSystem.h"
#include "Engine.h"
#include "Tools/EngineLog.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h" //include glad/glfw/etc.

namespace Engine
{
	void WindowSystem::makeWindowPrimary(const sp<Window>& window)
	{
		onPrimaryWindowChangingEvent.broadcast(focusedWindow, window);

		onWindowLosingOpenglContext.broadcast(focusedWindow);
		if (window)
		{
			glfwMakeContextCurrent(window->get());

			focusedWindow = window; //make sure assignment happens before event broadcast

			onWindowAcquiredOpenglContext.broadcast(window);
		}
		else
		{
			focusedWindow = nullptr;
		}

	}

	void WindowSystem::tick(float /*deltaSec*/)
	{
		glfwPollEvents();

		if (focusedWindow)
		{
			if (focusedWindow->shouldClose())
			{
				if (onFocusedWindowTryingToClose.numBound() > 0)
				{
					//NOTE: below may result in focus window changing; making a sp copy per tick isn't cheap so be careful
					onFocusedWindowTryingToClose.broadcast(focusedWindow);
				}
				else
				{
					log("WindowSystem", LogLevel::LOG_ERROR, "Window System : Primary window requesting close, shutting down game.");
					EngineBase::get().startShutdown();
				}
			}
		}
	}

	void WindowSystem::handlePostRender()
	{
		//will need to do this for all windows that were rendered to if supporting more than a single window
		if (focusedWindow)
		{
			glfwSwapBuffers(focusedWindow->get());
		}
	}

	void WindowSystem::initSystem()
	{
		EngineBase::get().subscribePostRender(sp_this());
	}
}


