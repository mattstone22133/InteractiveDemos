#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file is used to include glfw and opengl so that they can be used. Since this is different, depending on the build, that is 
// sorted out here.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include<GLFW/glfw3.h> //this must come before glad include, otherwise get macro redifintion issues; likely related to including windows.h; https://github.com/Dav1dde/glad/issues/283 glad2 apparently resolves this.

#ifdef HTML_BUILD //ONLY HTML
#include <emscripten/emscripten.h>
#include <GLES2/gl2.h>
#else //DESKTOP ONLY
#include<glad/glad.h>
#endif