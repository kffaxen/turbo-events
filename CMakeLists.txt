cmake_minimum_required(VERSION 3.16)

# set the project name
project(TurboEvents)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

option(CLANG_TIDY "Run clang-tidy with the compiler." ON)

if(CLANG_TIDY)
    find_program(CLANG_TIDY_EXE_NAME
        NAMES "clang-tidy-12" "clang-tidy"
    )
    if(CLANG_TIDY_EXE_NAME)
        set(CLANG_TIDY_EXE "${CLANG_TIDY_EXE_NAME}")
        message(STATUS "Using clang-tidy: ${CLANG_TIDY_EXE}")
    endif()
endif()

# add the source, library and include directories so that cmake can find them
add_subdirectory(src)
add_subdirectory(lib)
