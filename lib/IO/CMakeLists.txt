
find_package(XercesC REQUIRED)
target_link_libraries(turboevents_main PRIVATE "${XercesC_LIBRARIES}")
