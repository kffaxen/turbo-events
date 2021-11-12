# add the executable
add_executable(turboevents_main main.cpp)

set_target_properties(turboevents_main PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
)

#set the name of the program
set_target_properties(turboevents_main PROPERTIES OUTPUT_NAME turboevents)

target_link_libraries(turboevents_main PRIVATE turboevents)

target_compile_options(turboevents_main PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

find_package(gflags COMPONENTS nothreads_static)
target_link_libraries(turboevents_main PRIVATE gflags)

find_package(XercesC REQUIRED)
target_link_libraries(turboevents_main PRIVATE "${XercesC_LIBRARIES}")
