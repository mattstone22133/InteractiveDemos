cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(LIB_NAME Engine)
project(${LIB_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/test/*.h)

add_library(
    ${LIB_NAME} STATIC
    ${sources}
)

# todo should separate the includes from source files (and potentialy private/public) folders
target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

#this is how you can add preprocessor macros within the build
target_compile_definitions(${LIB_NAME} PUBLIC ENGINE_VERSION=1)

set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

