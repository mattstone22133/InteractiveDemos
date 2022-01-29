cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(EXECUTABLE_NAME SeparatingAxisTheoremDemo)

project(${EXECUTABLE_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(${EXECUTABLE_NAME} ${sources})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC TutorialEngine)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC OldOpenGL3Project)

#------------------------
# don't think I need the preload assets for this demo, since I don't plan to port the models part, nor use text. 
# Avoiding preload will keep file size small. 
# But if tutorial engine ever needs someting from its preload assets to run, we will have to enable the below
#------------------------
# copy preload assets for tutorial engine (eg bitmap font textures) to relative path for executable
# file(COPY ${TUTORIAL_ENGINE_PRELOAD_SOURCE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${TUTORIAL_ENGINE_PRELOAD_DESTINATION}) 

# if html build, instruct files to be preloaded, so that they can be loaded (eg textures)
include(${SHARED_CMAKE_UTILITES_DIR}/UtilitiesHTML.cmake) #variable should be defined at toplevel cmakelists.txt
#HTMLPreloadDirectory(${EXECUTABLE_NAME} "${CMAKE_CURRENT_BINARY_DIR}/PreloadAssets/@PreloadAssets") #@ symbol renames path on left of @ to path on right of @; ie old/path@newpath
HTMLUseTemplateHtmlFile(${EXECUTABLE_NAME} "${CMAKE_CURRENT_LIST_DIR}/html_output_template.html")

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)
    
# change html build output so that it is runnable via server
if(HTML_BUILD) 
    #message(STATUS "Separating Axis Theorm Building HTML")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

target_compile_definitions(${EXECUTABLE_NAME} PUBLIC WITH_SAT_DEMO_BUILD) #flag to enable/disable files
