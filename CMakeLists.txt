cmake_minimum_required(VERSION 3.16)

# set the project name
project(TurboEvents)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# add the source, library and include directories so that cmake can find them
add_subdirectory(src)
add_subdirectory(lib)
