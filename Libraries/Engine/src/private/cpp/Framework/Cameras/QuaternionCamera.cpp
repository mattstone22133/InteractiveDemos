#include "Framework/Cameras/QuaternionCamera.h"
#include "Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h"
#include "EngineSystems/WindowSystem/WindowWrapper.h"
#include "EngineSystems/WindowSystem/WindowSystem.h"

namespace Engine
{
	QuaternionCamera::QuaternionCamera()
	{
		cursorModeKey = GLFW_KEY_ESCAPE;
		updateBasisVectors();
	}

	void QuaternionCamera::tick(float dt_sec)
	{
		const sp<Window>& windowWrapper = WindowSystem::get().getPrimaryWindow();
		GLFWwindow* window = windowWrapper ? windowWrapper->get() : nullptr;
		if (!window)
		{
			return;
		}

		//in real applications one should hook into the associated events with callbacks rather than polling every frame

		bool bCTRL = glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
		bool bALT = glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
		bool bSHIFT = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

		////////////////////////////////////////////////////////
		// check mouse movement
		////////////////////////////////////////////////////////
		if (bEnableOrbitModeSwitch) { lookMode = bALT ? LookMode::Orbit : LookMode::Free; }

		int windowHeight, windowWidth;
		double x, y;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		glfwGetCursorPos(window, &x, &y);

		x += double(windowWidth) / 0.5;
		y += double(windowHeight) / 0.5;

		glm::vec2 deltaMouse{ float(x - lastX), float(y - lastY) };
		lastX = x;
		lastY = y;
		handleMouseMoved(dt_sec, deltaMouse);

		////////////////////////////////////////////////////////
		// Cursor mode
		////////////////////////////////////////////////////////
		static bool bJustPressedCursorMode = false;
		if (glfwGetKey(window, cursorModeKey) == GLFW_PRESS)
		{
			if (!bJustPressedCursorMode)
			{
				bCursorMode = !bCursorMode;
				glfwSetInputMode(window, GLFW_CURSOR, bCursorMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

				//only flip once until they release
				bJustPressedCursorMode = true;
			}
		}
		else { bJustPressedCursorMode = false; }


		////////////////////////////////////////////////////////
		// handle movement
		////////////////////////////////////////////////////////
		glm::vec3 inputVector{ 0.f };
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			inputVector += -w_axis;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			inputVector -= -w_axis;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			inputVector += u_axis;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			inputVector += -u_axis;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		{
			inputVector += v_axis;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			inputVector += -v_axis;
		}
		if (glm::length2(inputVector) != 0.f)
		{
			inputVector = glm::normalize(inputVector);
			pos += inputVector * cameraSpeed * dt_sec * (bCTRL ? 10.f : 1.f) * (bSHIFT ? 0.1f : 1.f);
		}

		////////////////////////////////////////////////////////
		// handle roll
		////////////////////////////////////////////////////////
		float rollDirection = 0.f;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			rollDirection += -1.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			rollDirection += 1.0f;
		}
		if (rollDirection != 0.f)
		{
			glm::quat roll = glm::angleAxis(rollDirection * rollSpeed * dt_sec * (bSHIFT ? 0.25f : 1.f), -w_axis);
			rotation = roll * rotation;
			updateBasisVectors();
		}
	}

	void QuaternionCamera::handleMouseMoved(float dt_sec, const glm::vec2& deltaMouse)
	{
		if (!bCursorMode)
		{
			glm::vec3 orbitPoint{ 0.f };
			if (lookMode == LookMode::Orbit)
			{
				orbitPoint = (orbitDistance * getFront()) + getPosition();
			}

			glm::vec3 uvPlaneVec = u_axis * deltaMouse.x;
			uvPlaneVec += v_axis * -deltaMouse.y;

			float rotationMagnitude = glm::length(uvPlaneVec);
			if (rotationMagnitude == 0.0f) { return; }
			uvPlaneVec = glm::normalize(uvPlaneVec);

			glm::vec3 rotationAxis = glm::normalize(glm::cross(uvPlaneVec, w_axis));
			glm::quat deltaQuat = glm::angleAxis(dt_sec * mouseSensitivity * rotationMagnitude, rotationAxis);
			assert(!anyValueNAN(deltaQuat));

			rotation = deltaQuat * rotation;
			updateBasisVectors();

			if (lookMode == LookMode::Orbit)
			{
				pos = orbitPoint + (orbitDistance * -getFront());
			}
		}
	}

	void QuaternionCamera::updateBasisVectors()
	{
		u_axis = rotation * glm::vec3{ 1,0, 0 }; //perhaps should normalize to be extra safe from fp imprecision ?
		v_axis = rotation * glm::vec3{ 0,1, 0 };
		w_axis = rotation * glm::vec3{ 0,0,1 };

		assert(!anyValueNAN(u_axis));
		assert(!anyValueNAN(v_axis));
		assert(!anyValueNAN(w_axis));
	}


}
