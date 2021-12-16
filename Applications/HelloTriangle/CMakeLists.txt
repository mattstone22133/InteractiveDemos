cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(EXECUTABLE_NAME HelloTriangle)

project(${EXECUTABLE_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(${EXECUTABLE_NAME} ${sources})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC Engine)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# change html build output so that it is runnable.
if(CMAKE_EXECUTABLE_SUFFIX STREQUAL ".js") #todo update to HTML_BUILD cmd after things get working
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

