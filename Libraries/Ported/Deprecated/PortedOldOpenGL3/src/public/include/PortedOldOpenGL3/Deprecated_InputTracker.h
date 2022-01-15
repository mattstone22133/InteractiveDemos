#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_OLD_OPENGL3_PROJECT_BUILD

#include <Utils/Platform/OpenGLES2/PlatformOpenGLESInclude.h>
//#include<glad/glad.h> //include opengl headers, so should be before anything that uses those headers (such as GLFW)
//#include<GLFW/glfw3.h>
#include <set>
#include <stack>

class Deprecated_InputTracker final
{
public:
	void updateState(GLFWwindow* window);

	bool isKeyJustPressed(GLFWwindow* window, int key);
	bool isKeyDown(GLFWwindow* window, int key);

	bool isMouseButtonJustPressed(GLFWwindow* window, int button);
	bool isMouseButtonDown(GLFWwindow* window, int button);
private:
	std::set<int> keysCurrentlyPressed;
	std::set<int> mouseButtonsCurrentlyPressed;

};

#endif //WITH_OLD_OPENGL3_PROJECT_BUILD
