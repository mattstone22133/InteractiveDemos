cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(EXECUTABLE_NAME Engine)

project(${EXECUTABLE_NAME} CXX)

#todo pickup here making the engine a library
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

ADD_LIBRARY{${EXECUTABLE_NAME} ${sources}}

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)