#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//	DISCLAIMER: THIS IS OLD PORTED CODE
//
//	THE CODE QUALITY IS LACKING
//    PORTED MOSTLY AS-IS
//
///////////////////////////////////////////////////////////////////////////////////////

#ifdef WITH_SAT_DEMO_BUILD

struct GLFWwindow;

class ISATDemo
{
public:
	ISATDemo(int width, int height) {}
	virtual ~ISATDemo() {}
	virtual void tickGameLoop(GLFWwindow* window) = 0;
	virtual void handleModuleFocused(GLFWwindow* window) = 0;

	//added to make demo more intuitive.
	virtual void populateUI() = 0;

	virtual bool bShowCameraMoveButton() const { return true; }
};
#endif //WITH_SAT_DEMO_BUILD
