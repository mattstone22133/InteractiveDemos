include(FetchContent)

macro(LinkKLEIN TARGET ACCESS)
    FetchContent_Declare(
        klein
        GIT_REPOSITORY https://github.com/jeremyong/klein
        GIT_TAG v2.2.1
    )

    FetchContent_GetProperties(klein)

    if (NOT klein_POPULATED)
        FetchContent_Populate(klein)
    endif()

    # only include the public headers, don't need full project. See README.md for the git repository
    target_include_directories(${TARGET} ${ACCESS} ${klein_SOURCE_DIR}/public)

endmacro()
