cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

message(STATUS "------------------------------------------------------------------------------------")

set(EXECUTABLE_NAME InteractiveSandbox)
project("${EXECUTABLE_NAME}")

# --------------------------------------------------------------
# global variables
# --------------------------------------------------------------
if (CMAKE_EXECUTABLE_SUFFIX STREQUAL ".js")
	set(HTML_BUILD True CACHE BOOL "whether this is an html build" FORCE) #TODO rename flag to emscripten_html_build 
	add_compile_definitions(HTML_BUILD)
else()
	set(HTML_BUILD False CACHE BOOL "whether this is an html build" FORCE) 
endif()
message(STATUS "HTML_BUILD Value: ${HTML_BUILD}")

# --------------------------------------------------------------
# Set up libraries
# --------------------------------------------------------------
# set up the engine library for all applications to link against
add_subdirectory("Libraries/Engine")
add_subdirectory("Libraries/TutorialEngine")

# --------------------------------------------------------------
# Set up applications
# --------------------------------------------------------------
add_compile_definitions(GLFW_INCLUDE_NONE) #stop GLFW and GLAD conflicts on linux; include order does not fix it on linux. Must be defined before include glfw; must not be defined when compiling glfw.

add_subdirectory("Applications/BuildSystemTest")
add_subdirectory("Applications/HelloTriangle")
add_subdirectory("Applications/BarycentricCoordinates")

message(STATUS " ------------------------------------------------------------------------------------")
