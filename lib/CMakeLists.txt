add_library(turboevents turboevents.cpp)

set_target_properties(turboevents PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
)

target_include_directories(turboevents PUBLIC ${TurboEvents_SOURCE_DIR}/include)
