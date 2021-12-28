cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(LIB_NAME Engine)

project(${LIB_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_library(
    ${LIB_NAME} 
	STATIC 
    ${sources} "src/public/include/ThirdParty/Imgui/DesktopGLES2LoaderWorkaround.h")

# set up the include directories
target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/public/include/")

#this is how you can add preprocessor macros within the build
target_compile_definitions(${LIB_NAME} PUBLIC ENGINE_VERSION=1)

set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Dependencies ===========================================================================================
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake) 
#===========================================================================================
# GLAD CONFIGURATION Refer to https://github.com/Dav1dde/glad for more information)
# Configure GLAD loader generation variables (lines below pulled from the GLAD top-level
# CMakeLists.txt file, with slight modifications.
# Refer to https://github.com/Dav1dde/glad for more information)
set(GLAD_OUT_DIR        "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Output directory" FORCE)
set(GLAD_PROFILE        "compatibility" CACHE STRING "OpenGL profile" FORCE)
set(GLAD_API            "gles2=2" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest" FORCE)
set(GLAD_GENERATOR      "c" CACHE STRING "Language to generate the binding for" FORCE)
set(GLAD_EXTENSIONS     "" CACHE STRING "Path to extensions file or comma separated list of extensions, if missing all extensions are included" FORCE)
set(GLAD_SPEC           "gl" CACHE STRING "Name of the spec" FORCE)
set(GLAD_ALL_EXTENSIONS OFF CACHE BOOL "Include all extensions instead of those specified by GLAD_EXTENSIONS" FORCE)
set(GLAD_NO_LOADER      OFF CACHE BOOL "No loader" FORCE)
set(GLAD_REPRODUCIBLE   OFF CACHE BOOL "Reproducible build" FORCE)
#===========================================================================================
# link dependencies
if(NOT HTML_BUILD)
	#message(STATUS "Engine Desktop Build")

	#only build this on desktop; emscripten does not need to build glfw manually, it ships with its own version
	include(${CMAKE_DIR}/LinkGLFW.cmake) 
	LinkGLFW(${LIB_NAME} PUBLIC)

	#only build this on desktop; emscripten will use the headers included with emscripten expose opengl functions
	include(${CMAKE_DIR}/LinkGLAD.cmake) 
	LinkGLAD(${LIB_NAME} PUBLIC)
else()
	message(STATUS "Engine HTML Build: adding html linker options")
	target_link_options(${LIB_NAME} PUBLIC -s USE_GLFW=3)	#link in emscripten glfw3; note that -lglfw is for the old glfw2
	target_link_options(${LIB_NAME} PUBLIC -lopenal)		#link in emscripten OpenAL for audio.
endif()

# add in glm math functions to all builds
include(${CMAKE_DIR}/LinkGLM.cmake)
LinkGLM(${LIB_NAME} PUBLIC)

include(${CMAKE_DIR}/LinkSTB.cmake)
LinkSTB(${LIB_NAME} PUBLIC)

# imgui is specific to engine project because of work arounds and is in a director relative to engine library.
include(${CMAKE_CURRENT_LIST_DIR}/CMake/EngineLinkImgui.cmake)
EngineLinkIMGUI(${LIB_NAME} PUBLIC)
	

# End Dependencies ===========================================================================================

#set warnings to be compile errors
include(${CMAKE_DIR}/WarningsAsErrors.cmake)
WarningsAsErrors(${LIB_NAME})