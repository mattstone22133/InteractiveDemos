
///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_OLD_OPENGL3_PROJECT_BUILD

#include "PortedOldOpenGL3/Deprecated_CameraFPS.h"
#include <iostream>

//local to .cpp 
namespace
{
	//glfw callbacks are a construct and making registering a bound obj to a member function hard.
	//I'ev opted to just assum there's only every one glfw callback handler that is set statically
	//its memory is cleaned up in camera dtors and can only be set via this .cpp.
	//I suppose a more elegant solution would be to have a static map that allows callbacks for
	//different windows, but I'll implement that if needed. The idea would be you use the window pointer
	//to look into the map and find the appropriate camera handler.
	Deprecated_CameraFPS* glfwCallbackHandler = nullptr;

	void mouseCallback(GLFWwindow* /*window*/, double xpos, double ypos)
	{
		if (glfwCallbackHandler && !glfwCallbackHandler->isInCursorMode())
		{
			glfwCallbackHandler->mouseMoved(xpos, ypos);
		}
	}

	void mouseLeftCallback(GLFWwindow* /*window*/, int enter)
	{
		if (glfwCallbackHandler)
		{
			glfwCallbackHandler->windowFocusedChanged(enter);
		}
	}

	void scrollCallback(GLFWwindow* /*window*/, double xOffset, double yOffset)
	{
		if (glfwCallbackHandler && !glfwCallbackHandler->isInCursorMode())
		{
			glfwCallbackHandler->mouseWheelUpdate(xOffset, yOffset);
		}
	}
}

void Deprecated_CameraFPS::exclusiveGLFWCallbackRegister(GLFWwindow* window)
{
	if (window)
	{
		glfwCallbackHandler = this;
		glfwSetCursorPosCallback(window, &mouseCallback);
		glfwSetCursorEnterCallback(window, &mouseLeftCallback);
		glfwSetScrollCallback(window, &scrollCallback);
	}
	else
	{
		std::cerr << "warning: null window passed when attempting to register for glfw mouse callbacks" << std::endl;
	}
}


Deprecated_CameraFPS::Deprecated_CameraFPS(float inFOV, float inYaw, float inPitch) :
	worldUp_n(0.f, 1.f, 0.f),
	FOV(inFOV),
	yaw(inYaw),
	pitch(inPitch)
{
	calculateEulerAngles();
}

Deprecated_CameraFPS::~Deprecated_CameraFPS()
{
	//clean up what's listening to the glfw window callbacks.
	if (glfwCallbackHandler == this)
	{
		glfwCallbackHandler = nullptr;
	}
}

glm::mat4 Deprecated_CameraFPS::getView() const
{
	//this creates a view matrix from two matrices.
	//the first matrix (ie the matrix on the right which gets applied first) is just translating points by the opposite of the camera's position.
	//the second matrix (ie matrix on the left) is a matrix where the first 3 rows are the U, V, and W axes of the camera.
	//		The matrix with the axes of the camera is a change of basis matrix.
	//		If you think about it, what it does to a vector is a dot product with each of the camera's axes.
	//		Since the camera axes are all normalized, what this is really doing is projecting the vector on the 3 axes of the camera!
	//			(recall that a vector dotted with a normal vector is equivalent of getting the scalar projection onto the normal)
	//		So, the result of the projection is the vector's position in terms of the camera position.
	//		Since we're saying that the camera's axes align with our NDC axes, we get coordinates in terms x y z that are ready for clipping and projection to NDC 
	//So, ultimately the lookAt matrix shifts points based on camera's position, then projects them onto the camera's axes -- which are aligned with OpenGL's axes 

	glm::vec3 cameraAxisW = glm::normalize(cameraPosition - (cameraFront_n + cameraPosition)); //points towards camera; camera looks down its -z axis (ie down its -w axis)
	glm::vec3 cameraAxisU = glm::normalize(glm::cross(worldUp_n, cameraAxisW)); //this is the right axis (ie x-axis)
	glm::vec3 cameraAxisV = glm::normalize(glm::cross(cameraAxisW, cameraAxisU));
	//make each row in the matrix a camera's basis vector; glm is column-major
	glm::mat4 cameraBasisProjection(1.f);
	cameraBasisProjection[0][0] = cameraAxisU.x;
	cameraBasisProjection[1][0] = cameraAxisU.y;
	cameraBasisProjection[2][0] = cameraAxisU.z;

	cameraBasisProjection[0][1] = cameraAxisV.x;
	cameraBasisProjection[1][1] = cameraAxisV.y;
	cameraBasisProjection[2][1] = cameraAxisV.z;

	cameraBasisProjection[0][2] = cameraAxisW.x;
	cameraBasisProjection[1][2] = cameraAxisW.y;
	cameraBasisProjection[2][2] = cameraAxisW.z;
	glm::mat4 cameraTranslate = glm::translate(glm::mat4(1.f), -1.f * cameraPosition);

	glm::mat4 view = cameraBasisProjection * cameraTranslate;
	//glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, worldUp);

	return view;
}

namespace DecrepcatedCameraFPSGlobals
{
	//some hacks I made for porting old code for SAT
	bool bTurnWhileRightClickedMode = false;
	bool bRightClickHeld = false;
}

void Deprecated_CameraFPS::mouseMoved(double xpos, double ypos)
{
	if (refocused)
	{
		lastX = xpos;
		lastY = ypos;
		refocused = false;
	}

	if (DecrepcatedCameraFPSGlobals::bTurnWhileRightClickedMode)
	{
		if (!DecrepcatedCameraFPSGlobals::bRightClickHeld)
		{
			refocused = true;
			return;
		}
	}

	double xOff = xpos - lastX;
	double yOff = lastY - ypos;

	xOff *= mouseSensitivity;
	yOff *= mouseSensitivity;

	yaw += static_cast<float>(xOff);
	pitch += static_cast<float>(yOff);

	lastX = xpos;
	lastY = ypos;

	if (pitch > 89.0f)
		pitch = 89.f;
	else if (pitch < -89.0f)
		pitch = -89.f;

	calculateEulerAngles();
}

void Deprecated_CameraFPS::windowFocusedChanged(int focusEntered)
{
	refocused = focusEntered;
}

void Deprecated_CameraFPS::mouseWheelUpdate(double /*xOffset*/, double yOffset)
{
	if (FOV >= 1.0f && FOV <= 45.0f)
		FOV -= static_cast<float>(yOffset);
	if (FOV <= 1.0f)
		FOV = 1.0f;
	if (FOV >= 45.0)
		FOV = 45.0f;
}

void Deprecated_CameraFPS::handleInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPosition -= cameraFront_n * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPosition += cameraFront_n * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//the w basis vector is the -cameraFront
		glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp_n, -cameraFront_n));
		cameraPosition += cameraRight * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//the w basis vector is the -cameraFront
		glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp_n, -cameraFront_n));
		cameraPosition -= cameraRight * cameraSpeed * deltaTime;
	}
}

void Deprecated_CameraFPS::setPosition(float x, float y, float z)
{
	cameraPosition.x = x;
	cameraPosition.y = y;
	cameraPosition.z = z;
}

void Deprecated_CameraFPS::setPosition(glm::vec3 newPos)
{
	cameraPosition = newPos;
}

void Deprecated_CameraFPS::setYaw(float inYaw)
{
	yaw = inYaw;
	calculateEulerAngles();
}

void Deprecated_CameraFPS::setPitch(float inPitch)
{
	pitch = inPitch;
	calculateEulerAngles();
}

void Deprecated_CameraFPS::setSpeed(float speed)
{
	cameraSpeed = speed;
}

const glm::vec3 Deprecated_CameraFPS::getRight() const
{
	glm::vec3 cameraRight_n = glm::normalize(glm::cross(worldUp_n, -cameraFront_n));
	return cameraRight_n;
}

const glm::vec3 Deprecated_CameraFPS::getUp() const
{
	glm::vec3 cameraRight_n = glm::normalize(glm::cross(worldUp_n, -cameraFront_n));
	glm::vec3 cameraUp_n = glm::normalize(glm::cross(-cameraFront_n, cameraRight_n));
	return cameraUp_n;
}

void Deprecated_CameraFPS::setCursorMode(bool inCursorMode)
{
	cursorMode = inCursorMode;
	if (!cursorMode)
	{
		//don't gitter camera
		refocused = true;
	}
}

void Deprecated_CameraFPS::calculateEulerAngles()
{
	glm::vec3 camDir;
	camDir.y = sin(glm::radians(pitch));
	camDir.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	camDir.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	camDir = glm::normalize(camDir); //make sure this is normalized for when user polls front
	cameraFront_n = camDir;
}

#endif //WITH_OLD_OPENGL3_PROJECT_BUILD
