include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(rvc_tests
    test_grid.cpp
    test_rvc_state.cpp
    test_command_dispatcher.cpp
    test_renderer.cpp
)
target_include_directories(rvc_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(rvc_tests PRIVATE rvc_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(rvc_tests)
