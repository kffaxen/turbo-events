# add the executable
add_executable(TurboEvents main.cpp)

# add dependency on the library
target_link_libraries(TurboEvents PRIVATE turboevents)
target_include_directories(TurboEvents PRIVATE ${PROJECT_SOURCE_DIR}/lib)
