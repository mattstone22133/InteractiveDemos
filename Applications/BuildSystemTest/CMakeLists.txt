cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(EXECUTABLE_NAME BuildSystemTest)

project(${EXECUTABLE_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(${EXECUTABLE_NAME} ${sources})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC Engine)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

if(CMAKE_EXECUTABLE_SUFFIX STREQUAL ".js")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()


