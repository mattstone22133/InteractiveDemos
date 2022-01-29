cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(LIB_NAME OldOpenGL3Project)

project(${LIB_NAME} CXX)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp src/*.h)

#prepare copy assets to build directory
#set(ProjectOpenGL3SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/PreloadAssets/ CACHE BOOL "directory with tutorial engine preload assets" FORCE) 
#set(ProjectOpenGL3DESTINATION /PreloadAssets/${LIB_NAME} CACHE BOOL "the subdirectory for the current building executable" FORCE) 
#file(COPY ${ProjectOpenGL3SOURCE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${ProjectOpenGL3DESTINATION})


add_library(
    ${LIB_NAME} STATIC 
    ${sources}
)

# set up the include directories
target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/public/include/")

# add engine library as a dependency
target_link_libraries(${LIB_NAME} PUBLIC Engine)
target_link_libraries(${LIB_NAME} PUBLIC TutorialEngine)

set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

#set warnings to be compile errors
include(${SHARED_CMAKE_UTILITES_DIR}/WarningsAsErrors.cmake)
WarningsAsErrors(${LIB_NAME})

target_compile_definitions(${LIB_NAME} PUBLIC WITH_OLD_OPENGL3_PROJECT_BUILD) #flag to enable/disable files
