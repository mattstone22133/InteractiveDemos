
#pragma once

#include<cstdint>
#include <utility>
#include <unordered_map>

#include "Event.h"
#include "GameObjectBase.h"

struct GLFWwindow;

namespace Engine
{
	/*
		A lightweight wrapper for GLFWwindow.

		Window Invariants Requirements:
			-window creation is done on a single thread;		static reference counting for startup/shutdown is not mutex guarded
	*/
	class Window : public GameObjectBase,
		public IEventSubscriber
	{
		//GLFW management
	private:
		static void startUp();
		static void tryShutDown();

	public:
		Event<double /*x*/, double /*y*/> cursorPosEvent;
		Event<int /*Enter*/> mouseLeftEvent;
		Event<double /*xOffset*/, double /*yOffset*/> scrollChanged;
		Event<int /*newWidth*/, int /*newHeight*/> framebufferSizeChanged;

		/* mods: GLFW_MOD_SHIFT, GLFW_MOD_CONTROL,GLFW_MOD_ALT, GLFW_MOD_SUPER
		   actions: GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT (kb only) */
		Event<int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/> onKeyInput;
		Event<int /*button*/, int /*action*/, int /*mods*/> onMouseButtonInput;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// These are provided for API's that bindings to the raw-callbacks to glfw window; since we use that they cannot register a callback
		// So, we forward the callbacks to those API's by directly calling their callbacks.
		// WARNING: Do not use this callbacks! prefer "onKeyInput" unless you're dealing with a c api that requires this be registered to glfw.
		Event<GLFWwindow* /*window*/, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/> onRawGLFWKeyCallback;
		Event<GLFWwindow* /*window*/, unsigned int /*c*/> onRawGLFWCharCallback;
		Event<GLFWwindow* /*window*/, double /*xoffset*/, double /*yoffset*/> onRawGLFWScrollCallback;
		Event<GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/> onRawGLFWMouseButtonCallback;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//window instances
	public:
		Window(uint32_t width, uint32_t height);
		virtual void postConstruct() override;
		~Window();

		inline GLFWwindow* get() { return window; }
		void markWindowForClose(bool bClose);
		bool shouldClose();
		float getAspect();
		std::pair<int, int> getFramebufferSize();
		bool isActiveContext() const;

		void setViewportToWindowSize();

	private:
		void handleFramebufferSizeChanged(int width, int height);

	private:
		GLFWwindow* window;
	};

}