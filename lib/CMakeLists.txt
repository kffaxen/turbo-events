add_library(turboevents turboevents.cpp)

set_target_properties(turboevents PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
)

target_include_directories(turboevents PUBLIC ${TurboEvents_SOURCE_DIR}/include)

target_compile_options(turboevents PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror
        -fno-omit-frame-pointer>
)

if(SANITIZERS AND COMPILER_HAS_UBSAN)
    target_compile_options(turboevents PUBLIC
        $<$<CONFIG:Debug>:-fsanitize=undefined,address>)
    target_link_options(turboevents PUBLIC
        $<$<CONFIG:Debug>:-fsanitize=undefined,address>)
endif()

find_package(pybind11 REQUIRED)
target_link_libraries(turboevents PUBLIC pybind11::embed)

if(WIN32)
    # FIXME: Add stack overflow detection on Windows.
    # target_sources(turboevents PRIVATE signals-windows.cpp)
else()
    target_sources(turboevents PRIVATE signals-unix.cpp)
endif()

add_subdirectory(IO)
