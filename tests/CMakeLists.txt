include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_grid        test_grid.cpp)
add_executable(test_rvcstate    test_rvcstate.cpp)
add_executable(test_dispatcher  test_dispatcher.cpp)

foreach(t test_grid test_rvcstate test_dispatcher)
    target_include_directories(${t} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${t} PRIVATE rvc_core GTest::gtest_main)
    add_test(NAME ${t} COMMAND ${t})
endforeach()
