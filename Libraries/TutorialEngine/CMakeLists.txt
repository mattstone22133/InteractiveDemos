cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(LIB_NAME TutorialEngine)

project(${LIB_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_library(
    ${LIB_NAME} STATIC 
    ${sources}
)

# set up the include directories
target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/public/include/")

# add engine library as a dependency
target_link_libraries(${LIB_NAME} PUBLIC Engine)

set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

