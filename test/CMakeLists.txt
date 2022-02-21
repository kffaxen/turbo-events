include(CTest)

include(ProcessorCount)
ProcessorCount(N)
add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -j${N} --output-on-failure)

# Test fixture to build the main binary which is required by most tests.
add_test(NAME test_build
  COMMAND "${CMAKE_COMMAND}"
             --build "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" -j${N}
	     --target turboevents_main)
set_tests_properties(test_build PROPERTIES FIXTURES_SETUP test_fixture)

# Each test to run, and their test_fixture to manage dependencies.
add_test(NAME xml_test
  COMMAND $<TARGET_FILE:turboevents_main>
            --input="hello,countdown"
            ${TurboEvents_SOURCE_DIR}/test/events1.xml
            ${TurboEvents_SOURCE_DIR}/test/events2.xml)
set_tests_properties(xml_test PROPERTIES FIXTURES_REQUIRED test_fixture)

add_test(NAME container_test
  COMMAND $<TARGET_FILE:turboevents_main>
            --script ${TurboEvents_SOURCE_DIR}/test/containerinput.py)
set_tests_properties(container_test PROPERTIES FIXTURES_REQUIRED test_fixture)

add_test(NAME xml_ctrl_test
  COMMAND $<TARGET_FILE:turboevents_main>
            --input="hello,countdown"
            --xml_ctrl="patient:id/:glucose_level/event:ts:value,patient:id/:meal/event:ts:type:carbs"
            ${TurboEvents_SOURCE_DIR}/test/events1.xml
            ${TurboEvents_SOURCE_DIR}/test/events2.xml
            ${TurboEvents_SOURCE_DIR}/test/events3.xml)
set_tests_properties(xml_ctrl_test PROPERTIES FIXTURES_REQUIRED test_fixture)
