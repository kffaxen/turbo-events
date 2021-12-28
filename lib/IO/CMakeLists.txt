find_package(PkgConfig REQUIRED)
pkg_check_modules(kafka REQUIRED IMPORTED_TARGET rdkafka)
target_sources(turboevents PRIVATE KafkaOutput.cpp)
target_link_libraries(turboevents PUBLIC PkgConfig::kafka)

find_package(XercesC REQUIRED)
target_sources(turboevents PRIVATE XMLInput.cpp)
target_link_libraries(turboevents PUBLIC "${XercesC_LIBRARIES}")
