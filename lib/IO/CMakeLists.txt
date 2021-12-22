find_package(XercesC REQUIRED)
target_sources(turboevents PRIVATE XMLInput.cpp)
target_link_libraries(turboevents PUBLIC "${XercesC_LIBRARIES}") 
