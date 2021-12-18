--------------- TL;DR -----------------------
Create a folder for the build 
    mkdir build
Go into the build folder
    cd build
Use emcmake to read the cmake file
    emcmake cmake .. -G Ninja
Build the output (note, can do this like `cmake --build . --verbose` for debugging)
    cmake --build .
Navigate to a build folder, and test running it using something like `node outout.js` or `python -m http.server` and navigating to local host; 

----------------REQUIREMENTS-----------------

You will need to download and set up the ninja build system.
Emscripten header files (GLFW/glfw3.h, GLES2/gl2.h) can be included and compilewhen using Ninja -- but it appears those headers are not found when using a visual studio generator
add it to environmental varibales path so cmake can find it when using `-G Ninja`
https://github.com/ninja-build/ninja/releases/tag/v1.10.2 is the link I used

You will need to have emscripten set up and be able to compile files using emcc. 

`emsdk` version `2.0.34` was used locally to compile to HTML; I have not made this a requirement but if issues arise, falling back to that sdk may help.
















----------------- NOTES ON EMSCRIPTEN VIRTUAL FILES ---------------------

Desktop applications have the files assets relative to the executable program. 
eg if application.exe needs to load texture.png, it could be structure liked
    application_folder
        application.exe
        PreloadAssets_folder
            texture.png
But javascript runs in a sandbox environment in the browser, and cannot load files from disk like the desktop.
so emscripten has a virtual file system to handle this. there are two approaches, embedding and preloading files. preloading is most efficient.
adding the following as a LINK_FLAG (to the target properties) can be used to instruct emscripten to preload a file.
    `--preload-file \"${FOLDER_PATH}\""` 
    it appears you need to repeat this --preload-file for each directory you add to be preloaded.
        `set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} --preload-file \"${FOLDER_PATH}\"")`
However, setting this up with cmake is precarious, hence the writing of this guide.

this error is common with many setups:      `path/to/file.txt` which is below the current directory `build/path/to/file`
        -FYI it appears this may should say "above" the current directory, rather than below. it is a confusing error.
            -see: https://github.com/emscripten-core/emscripten/issues/2656
    what this error is attempting to say -- is that a mapped file needs to be a within or a subdirectory of the build_folder.
    what is the build_folder?
        `cd CMakeListsDir`
        `mkdir build_folder`
        `cd build_folder`
        `cmake ..`              (or `emcmake cmake .. -G Ninja`)
        `cmake --build .`
    This means the following set up is an issues
        CMakeListDir
            Applications
                Project
                    src_folder
                    PreloadAssets_folder
        build_folder
            Applications
                Project
    In this case, the PreloadAssets_folder is not underneath the build_folder, so you will get the "below current directly" error.
    There are a couple ways to solve this
        1) if PreloadAssets_folder is copied to binary_dir, it will be copied into the build dir.
            -this means it is below the build_folder
            -which means it can be referenced
            -consider this test:
                file(COPY "${SHARED_CMAKE_UTILITES_DIR}/../tests" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)                     #copy to binary dir
                include(${SHARED_CMAKE_UTILITES_DIR}/UtilitiesHTML.cmake)                                                       #include helper that adds link option (see `set_target_properties` example above)
                HTMLPreloadDirectory(${EXECUTABLE_NAME} "./Applications/BarycentricCoordinates/tests/hello_world_file.txt")     #ultimately calls `set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} --preload-file \"${FOLDER_PATH}\"")`
            -above works, you can even just specify `tests` and it can still read the file contents within that folder.
                -BUT the application c++/c will have a different include path, compared to the destkop
                    HTML:       `FILE* file = fopen("./Applications/BarycentricCoordinates/tests/hello_world_file.txt", "rb");`
                    DESKTOP:`   FILE* file = fopen("tests/hello_world_file.txt", "rb");
            -so we can use option 2 instead, so that the desktop references consistent paths.
        2) use virtual file path remapping.
            the @ symbol is used to map folders OUTSIDE of the build_folder.
                `HTMLPreloadDirectory(${EXECUTABLE_NAME} "./Applications/Project@/")` 
                    -ie set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} --preload-file \"./Applications/Project@\" ")
                    -this will let us have the same include path in the application, but it is preloading all the files in our Project folder (eg src_folder)... which is bad

                `HTMLPreloadDirectory(${EXECUTABLE_NAME} "./Applications/Project/PreloadAssets_folder@/")`
                    -ie set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} --preload-file \"./Applications/Project/PreloadAssets_folder@/\" ")
                    - this somewhat works, but our application now must not include the PreloadAssets_folder in its path, making html and desktop again use different file paths
                
                you can rename a path by providing a name after the @ sign. eg this renames path/to/fork to ./spoon. path/to/fork@spoon
                    `HTMLPreloadDirectory(${EXECUTABLE_NAME} "./Applications/Project/PreloadAssets_folder/@PreloadAssets_folder")`
                        ie `set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${PreviousLinkFlags} --preload-file \"./Applications/Project/PreloadAssets_folder/@PreloadAssets_folder\" ")`
                        this appears to be the option to choose. it remaps the contents of PreloadAssets_folder folder, to a virtual folder accessed via ./PreloadAssets_folder
-------------------------------------------------------------------------




























