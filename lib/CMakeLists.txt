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
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

if(COMPILER_HAS_UBSAN)
    target_compile_options(turboevents PUBLIC
        $<$<CONFIG:Debug>:-fsanitize=undefined,address>)
    target_link_options(turboevents PUBLIC
        $<$<CONFIG:Debug>:-fsanitize=undefined,address>)
endif()

add_subdirectory(IO)
