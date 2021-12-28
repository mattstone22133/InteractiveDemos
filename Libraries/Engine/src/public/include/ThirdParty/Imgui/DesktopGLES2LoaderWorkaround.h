#pragma once

/*
reference this file: imgui_impl_opengl3.cpp


if you IMGUI_IMPL_OPENGL_ES2, it will skip glad on windows and try to include files only present on linux (gles2/gl.h)
 specifically this part

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h> //<- causes issues on windows with glad. file does not exist.


So, I am using a custom loader to force GLAD, and once glad has been initialized, set it so that imgui uses GLES2
the idea here is to not modify imgui code to get it to work
but it appears es2+glad combination is not a supported combination on windows.
so, using the custom loader feature to do this work around. this file will be included into imgui_impl_opengl3

*/
//1) use glad.
#include <glad/glad.h>

//2) have imgui now use 
#define IMGUI_IMPL_OPENGL_ES2

//3) since imgui_impl_opengl3 will not define IMGUI_IMPL_OPENGL_LOADER_GLAD if we have acustom loader, let's just define that to make things are as if we were using glad
//#define IMGUI_IMPL_OPENGL_LOADER_GLAD
