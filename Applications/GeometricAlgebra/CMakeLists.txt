cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(EXECUTABLE_NAME GeometricAlgebra)

project(${EXECUTABLE_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(${EXECUTABLE_NAME} ${sources})

#target_link_libraries(${EXECUTABLE_NAME} PUBLIC Engine) #automatically links this librarys ince TutorialEngine automatically links this library
target_link_libraries(${EXECUTABLE_NAME} PUBLIC TutorialEngine)

# copy preload assets for tutorial engine (eg bitmap font textures) to relative path for executable
#file(COPY ${TUTORIAL_ENGINE_PRELOAD_SOURCE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${TUTORIAL_ENGINE_PRELOAD_DESTINATION})

# if html build, instruct files to be preloaded, so that they can be loaded (eg textures)
include(${SHARED_CMAKE_UTILITES_DIR}/UtilitiesHTML.cmake) #variable should be defined at toplevel cmakelists.txt
#HTMLPreloadDirectory(${EXECUTABLE_NAME} "${CMAKE_CURRENT_BINARY_DIR}/PreloadAssets/@PreloadAssets") #@ symbol renames path on left of @ to path on right of @; ie old/path@newpath
#HTMLUseTemplateHtmlFile(${EXECUTABLE_NAME} "${CMAKE_CURRENT_LIST_DIR}/html_output_template.html")

# emscripten doesn't have SIMD support by default, it must be enabled. https://emscripten.org/docs/porting/simd.html
# this support looks like it is a proposal, rather than being fully supported.
# so I am disabling klein until I can investigate whether or not can safely use it in emscripten environment
include(${SHARED_CMAKE_UTILITES_DIR}/LinkKLEIN.cmake)
LinkKLEIN(${EXECUTABLE_NAME} PUBLIC)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)
    
# change html build output so that it is runnable via server
if(HTML_BUILD) 
    #message(STATUS "Building HTML Geometric Algebra")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    #message(STATUS "HTML: appending SIMD flag")
    #HTML_AppendLinkFlag(${EXECUTABLE_NAME} "-msimd128") #enable SIMD proposal so that we can compile klein (ERROR THIS ISN"T LINK FLAG")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msimd128") #enable SIMD proposal so that we can compile klein (limited browser support, so just do it for this project)
endif()

 