include(CTest)

include(ProcessorCount)
ProcessorCount(N)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -j${N})

# Test fixture to build the main binary which is required by most tests.
add_test(NAME test_build
  COMMAND "${CMAKE_COMMAND}"
             --build "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" -j${N}
	     --target turboevents_main)
set_tests_properties(test_build PROPERTIES FIXTURES_SETUP test_fixture)

# Each test to run, and their test_fixture to manage dependencies.
add_test(NAME xml_test
  COMMAND $<TARGET_FILE:turboevents_main>
            ${TurboEvents_SOURCE_DIR}/test/events1.xml
            ${TurboEvents_SOURCE_DIR}/test/events2.xml)
set_tests_properties(xml_test PROPERTIES FIXTURES_REQUIRED test_fixture)
