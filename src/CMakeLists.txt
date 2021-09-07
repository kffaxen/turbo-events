# add the executable
add_executable(turboevents_main main.cpp)

set_target_properties(turboevents_main PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

#set the name of the program
set_target_properties(turboevents_main PROPERTIES OUTPUT_NAME turboevents)

target_link_libraries(turboevents_main PRIVATE turboevents)

find_package(gflags COMPONENTS nothreads_static)
target_link_libraries(turboevents_main PRIVATE gflags)
