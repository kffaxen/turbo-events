# add the executable
add_executable(turboevents_main main.cpp)

#set the name of the program
set_target_properties(turboevents_main PROPERTIES OUTPUT_NAME turboevents)

target_link_libraries(turboevents_main PRIVATE turboevents)
