cmake_minimum_required(VERSION 3.16)
project(RvcSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

# ── Core library (world + command + render, no I/O — easily testable) ──────
add_library(rvc_core STATIC
    src/world/Grid.cpp
    src/world/RvcState.cpp
    src/command/CommandDispatcher.cpp
    src/render/Renderer.cpp
)
target_include_directories(rvc_core PUBLIC src)
target_link_libraries(rvc_core PUBLIC Threads::Threads)

# ── Main executable ────────────────────────────────────────────────────────
add_executable(rvc_simulator
    src/main.cpp
    src/demo/Demo.cpp
    src/network/Session.cpp
    src/network/TcpServer.cpp
)
target_include_directories(rvc_simulator PRIVATE src)
target_link_libraries(rvc_simulator PRIVATE rvc_core)

# ── clang-tidy (opt-in) ────────────────────────────────────────────────────
option(ENABLE_CLANG_TIDY "Run clang-tidy during build" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
    set(CLANG_TIDY_CMD "${CLANG_TIDY_EXE}" "--extra-arg=-std=c++17")
    set_target_properties(rvc_core      PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_CMD}")
    set_target_properties(rvc_simulator PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_CMD}")
endif()

# ── Tests ──────────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)
