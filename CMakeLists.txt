cmake_minimum_required(VERSION 3.16)

# set the project name
project(TurboEvents)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug'.")
  set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-fsanitize=undefined COMPILER_HAS_UBSAN)
# FIXME (CMake 3.19): Add COMPILER_HAS_ASAN check (requires linker flags).

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

option(DOXYGEN "Generate doxygen documentation." ON)

if(DOXYGEN)
    find_package(Doxygen
        REQUIRED dot
        OPTIONAL_COMPONENTS mscgen dia
    )
    doxygen_add_docs(doxygen ${PROJECT_SOURCE_DIR} ALL)
    # Doxygen is developer documentation for us, only build for debug builds.
    # I cannot get the "ALL" in the previous line from a generator expression
    # so toggle the target properties now that we have a target.
    set_target_properties(doxygen PROPERTIES EXCLUDE_FROM_ALL $<CONFIG:Release>)
endif()

# add the source, library and include directories so that cmake can find them
add_subdirectory(src)
add_subdirectory(lib)
