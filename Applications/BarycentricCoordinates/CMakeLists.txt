cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(EXECUTABLE_NAME BarycentricCoordinateDemo)

project(${EXECUTABLE_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(${EXECUTABLE_NAME} ${sources})

#target_link_libraries(${EXECUTABLE_NAME} PUBLIC Engine) #automatically links this librarys ince TutorialEngine automatically links this library
target_link_libraries(${EXECUTABLE_NAME} PUBLIC TutorialEngine)

# copy preload assets for tutorial engine (eg bitmap font textures) to relative path for executable
file(COPY ${TUTORIAL_ENGINE_PRELOAD_SOURCE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${TUTORIAL_ENGINE_PRELOAD_DESTINATION})

# if html build, instruct files to be preloaded, so that they can be loaded (eg textures)
include(${SHARED_CMAKE_UTILITES_DIR}/UtilitiesHTML.cmake) #variable should be defined at toplevel cmakelists.txt
HTMLPreloadDirectory(${EXECUTABLE_NAME} "./Applications/BarycentricCoordinates/PreloadAssets/@PreloadAssets") #@ symbol renames path on left of @ to path on right of @; ie old/path@newpath


set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)
    
# change html build output so that it is runnable via server
if(HTML_BUILD) 
    message(STATUS "Barycentry Coordinate Demo Building HTML")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

 