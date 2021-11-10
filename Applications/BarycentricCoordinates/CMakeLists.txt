cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(EXECUTABLE_NAME BarycentricCoordinateDemo)

project(${EXECUTABLE_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(${EXECUTABLE_NAME} ${sources})

#target_link_libraries(${EXECUTABLE_NAME} PUBLIC Engine) #automatically links this librarys ince TutorialEngine automatically links this library
target_link_libraries(${EXECUTABLE_NAME} PUBLIC TutorialEngine)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# change html build output so that it is runnable via server
if(HTML_BUILD) 
    message(STATUS "Barycentry Coordinate Demo Building HTML")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

