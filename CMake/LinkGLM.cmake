include(FetchContent)

macro(LinkGLM TARGET ACCESS)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/DependencyMaster/glm.git
        GIT_TAG 0.9.9.8
    )

    FetchContent_GetProperties(glm)

    if (NOT glm_POPULATED)
        FetchContent_Populate(glm)
    endif()

    target_compile_definitions(${TARGET} PUBLIC GLM_FORCE_SILENT_WARNINGS=1) #silence glm warnings about nameless structs
    target_include_directories(${TARGET} ${ACCESS} ${glm_SOURCE_DIR})
endmacro()
