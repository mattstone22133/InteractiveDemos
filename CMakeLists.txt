cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(EXECUTABLE_NAME InteractiveSandbox)
project("${EXECUTABLE_NAME}")

# --------------------------------------------------------------
# Set up libraries
# --------------------------------------------------------------
# set up the engine library for all applications to link against
add_subdirectory(engine)

# --------------------------------------------------------------
# Set up applications
# --------------------------------------------------------------
add_subdirectory("applications/BuildSystemTest")
