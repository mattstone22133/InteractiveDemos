
#include "EngineSystems/WindowSystem/WindowWrapper.h"

#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"

#include <stdexcept>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "Tools/EngineLog.h"

#define MAP_GLFWWINDOW_TO_WINDOWOBJ

static bool gladLoaded = false;

namespace Engine
{
	/**
		Simple concept to pimpl, but all windows share this implementation
		The goal is to hide the interface between the C api and the exposed C++ interface
		this class should not be useable outside of the this translation unit!
	*/
	class WindowStaticsImplementation
	{
	public:

		inline Window& findWindow(GLFWwindow* window)
		{
#ifdef MAP_GLFWWINDOW_TO_WINDOWOBJ
			auto winIter = windowMap.find(window);
			if (winIter != windowMap.end())
			{
				Window* winObj = winIter->second;
				return *winObj;
			}
#else 
			NOT_IMPLEMENTED;
#endif
			//fall through to return nothing if no window found
			throw std::runtime_error("FATAL: no window matching window from callback");
		}

		/** This is a raw pointer so it can be passed form Window's constructor */
		inline void trackWindow(GLFWwindow* rawWindow, Window* windowObj)
		{
#ifdef MAP_GLFWWINDOW_TO_WINDOWOBJ
			windowMap.insert({ rawWindow, windowObj });
			bindWindowCallbacks(rawWindow);
#else
			NOT_IMPLEMENTED;
#endif
		}

		inline void stopTrackingWindow(GLFWwindow* rawWindow)
		{
#ifdef MAP_GLFWWINDOW_TO_WINDOWOBJ
			windowMap.erase(rawWindow);
#else
			NOT_IMPLEMENTED;
#endif
		}
	private:
		void bindWindowCallbacks(GLFWwindow* window);

	private:
#ifdef MAP_GLFWWINDOW_TO_WINDOWOBJ
		std::unordered_map<GLFWwindow*, Window*> windowMap;
#else 
		std::vector<Window*> windows;
#endif

	public:
		uint32_t getWindowInstances() {
#ifdef MAP_GLFWWINDOW_TO_WINDOWOBJ
			return static_cast<uint32_t>(windowMap.size());
#else
			NOT_IMPLEMENTED;
#endif
		};
	};
	static WindowStaticsImplementation windowStatics;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// C CALLBACK MANAGEMENT
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static void c_callback_CursorPos(GLFWwindow* window, double xpos, double ypos)
	{
		Window& windowObj = windowStatics.findWindow(window);
		windowObj.cursorPosEvent.broadcast(xpos, ypos);
	}

	static void c_callback_CursorEnter(GLFWwindow* window, int enter)
	{
		Window& windowObj = windowStatics.findWindow(window);
		windowObj.mouseLeftEvent.broadcast(enter);
	}

	static void c_callback_Scroll(GLFWwindow* window, double xOffset, double yOffset)
	{
		Window& windowObj = windowStatics.findWindow(window);
		windowObj.scrollChanged.broadcast(xOffset, yOffset);
		windowObj.onRawGLFWScrollCallback.broadcast(window, xOffset, yOffset);
	}

	static void c_callback_FramebufferSize(GLFWwindow* window, int width, int height)
	{
		Window& windowObj = windowStatics.findWindow(window);
		windowObj.framebufferSizeChanged.broadcast(width, height);
	}

	static void c_callback_KeyCallback(GLFWwindow* window, int key, int scanecode, int action, int mods)
	{
		Window& windowObj = windowStatics.findWindow(window);
		windowObj.onKeyInput.broadcast(key, scanecode, action, mods);
		windowObj.onRawGLFWKeyCallback.broadcast(window, key, scanecode, action, mods);
	}

	static void c_callback_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		Window& windowObj = windowStatics.findWindow(window);
		windowObj.onMouseButtonInput.broadcast(button, action, mods);
		windowObj.onRawGLFWMouseButtonCallback.broadcast(window, button, action, mods);
	}

	static void c_callback_CharCallback(GLFWwindow* window, unsigned int c)
	{
		Window& windowObj = windowStatics.findWindow(window);
		windowObj.onRawGLFWCharCallback.broadcast(window, c);
	}

	///this must come after callbacks for proper definition order, otherwise forward declarations are going to be needed
	void WindowStaticsImplementation::bindWindowCallbacks(GLFWwindow* window)
	{
		//unbinding I believe is done implicitly by GLFW destroy window
		glfwSetCursorPosCallback(window, &c_callback_CursorPos);
		glfwSetCursorEnterCallback(window, &c_callback_CursorEnter);
		glfwSetScrollCallback(window, &c_callback_Scroll);
		glfwSetFramebufferSizeCallback(window, &c_callback_FramebufferSize);
		glfwSetKeyCallback(window, &c_callback_KeyCallback);
		glfwSetMouseButtonCallback(window, &c_callback_MouseButtonCallback);
		glfwSetCharCallback(window, &c_callback_CharCallback);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//STATIC GLFW MANAGEMENT
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//void APIENTRY c_api_errorMsgCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	void Window::startUp()
	{
		if (windowStatics.getWindowInstances() == 0)
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API); //WindowsOS will fail to create a window with this hint; it may be related to AMD cards not supporting gles2 via extensions like intel and nvidia does. https://www.saschawillems.de/blog/2015/04/19/using-opengl-es-on-windows-desktops-via-egl/
			glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API); //seems to be needed to get OpenGL ES context on WindowsOS.
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); //https://www.glfw.org/docs/3.3.2/window_guide.html#GLFW_OPENGL_PROFILE_attrib says gles must use anyprofile;  GLFW_OPENGL_ANY_PROFILE if the OpenGL profile is unknown ****or the context is an OpenGL ES*** context.
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //If OpenGL ES is requested, this hint is ignored. https://www.glfw.org/docs/3.3/window_guide.html#GLFW_OPENGL_FORWARD_COMPAT_hint
		}
	}

	static void post_context_init_setup()
	{
		if (windowStatics.getWindowInstances() == 0)
		{
#if ENABLE_GL_DEBUG_OUTPUT_GL43
			if (GAME_GL_MAJOR_VERSION >= 4 && GAME_GL_MINOR_VERSION >= 3)
			{
				ec(glEnable(GL_DEBUG_OUTPUT));
				ec(glDebugMessageCallback(MessageCallback, 0));
			}
			else
			{
				log("LogWindowStatics", LogLevel::LOG_ERROR, "Attempted to set message callback, but gl version too low");
			}
#endif //ENABLE_GL_DEBUG_OUTPUT_GL43
		}
	}

	void Window::tryShutDown()
	{
		if (windowStatics.getWindowInstances() == 0)
		{
			glfwTerminate();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Window Instances
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Window::Window(const uint32_t width, const uint32_t height)
	{
		startUp();

		window = glfwCreateWindow((int)width, (int)height, "OpenGL Window", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			throw std::runtime_error("FATAL: FAILED TO CREATE WINDOW");
		}

		//must be done everytime something is rendered to this window
		glfwMakeContextCurrent(window);
		post_context_init_setup();
		//glfwSwapInterval(0);

		//I believe GLAD proc address only needs to happen once
		if (!gladLoaded)
		{
			gladLoaded = true;
#if !HTML_BUILD //GLAD is not used for emscripten, instead use "#include <GLES2/gl2.h>" with emscripten; these headers are added via emcmake 
			if (!gladLoadGLES2Loader((GLADloadproc)&glfwGetProcAddress))
			{
				std::cerr << "Failed to set up GLAD" << std::endl;
				glfwTerminate();
				throw std::runtime_error("FATAL: FAILED GLAD LOAD PROC");
			}
#endif //!HTML_BUILD
		}

		windowStatics.trackWindow(window, this);

	}

	void Window::postConstruct()
	{
		//it is weird that it listens to its own delegate, but the alternative is to friend the
		//interface between the C api that calls the event. But I also wanted to test the system for
		//letting objects subscrib at construction. So this acts as sa test for that.
		framebufferSizeChanged.addWeakObj(sp_this(), &Window::handleFramebufferSizeChanged);
	}

	Window::~Window()
	{
		windowStatics.stopTrackingWindow(window);
		glfwDestroyWindow(window);
		tryShutDown();
	}

	void Window::markWindowForClose(bool bClose)
	{
		glfwSetWindowShouldClose(window, bClose);
	}

	bool Window::shouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	float Window::getAspect()
	{
		static bool bLoggedAspectError = false;
		int width, height;

		//unsure if this should be framebuffer size or actual window size
		glfwGetFramebufferSize(window, &width, &height);
		//glfwGetWindowSize(window, &width, &height);

		float aspect = static_cast<float>(width) / height;

		bool bSmallAspect = aspect - std::numeric_limits<float>::epsilon() < 0;
		if (aspect == 0.0f || std::isnan(aspect) || std::isinf(aspect) || bSmallAspect)
		{
			if (!bLoggedAspectError)
			{
				//taking care not to log every tick
				log("Window", LogLevel::LOG_ERROR, "Bad aspect!");
				bLoggedAspectError = true;
			}

			//make sure user can't do scale window in a way that has 0 as aspect in some sort.
			//assert(false);
			return 1.f; //pass a square aspect rather than hard crashing
		}
		bLoggedAspectError = false;
		return aspect;
	}

	std::pair<int, int> Window::getFramebufferSize()
	{
		//static bool bLoggedAspectError = false;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		return std::make_pair(width, height);
	}

	bool Window::isActiveContext() const
	{
		return glfwGetCurrentContext() == window;
	}

	void Window::setViewportToWindowSize()
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ec(glViewport(0, 0, width, height));
	}

	void Window::setWindowSize(const uint32_t width, const uint32_t height, const bool bUpdateViewport)
	{
		if (window)
		{
			glfwSetWindowSize(window, int(width), int(height));
		}
		if (bUpdateViewport)
		{
			setViewportToWindowSize();
		}
	}

	void Window::handleFramebufferSizeChanged(int width, int height)
	{
		//update view port to support resizing
		ec(glViewport(0, 0, width, height));
	}

}
