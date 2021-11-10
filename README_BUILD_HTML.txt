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




