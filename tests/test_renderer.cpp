#include <gtest/gtest.h>
#include <mutex>
#include <string>
#include <thread>
#include <chrono>

#include "render/Renderer.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

// ── buildFrame content ────────────────────────────────────────────────────────

TEST(RendererTest, FrameContainsHeader) {
    Grid     g = Grid::fromString("###\n# #\n###\n");
    RvcState s{1, 1, Direction::North};
    const std::string frame = Renderer::buildFrame(g, s);
    EXPECT_NE(frame.find("RVC Simulator"), std::string::npos);
}

TEST(RendererTest, FrameContainsDirectionLabel) {
    Grid     g{5, 5};
    RvcState s{2, 2, Direction::East};
    const std::string frame = Renderer::buildFrame(g, s);
    EXPECT_NE(frame.find("East"), std::string::npos);
}

TEST(RendererTest, FrameShowsCleanerStatus) {
    Grid     g{5, 5};
    RvcState s{1, 1, Direction::South};
    s.setCleanerOn(true);
    const std::string frame = Renderer::buildFrame(g, s);
    EXPECT_NE(frame.find("ON"), std::string::npos);
}

TEST(RendererTest, FrameShowsDustCount) {
    Grid g{5, 5};
    g.placeDust(0, 0);
    g.placeDust(1, 1);
    RvcState s{2, 2, Direction::North};
    const std::string frame = Renderer::buildFrame(g, s);
    EXPECT_NE(frame.find("2"), std::string::npos);
}

// ── Start/stop lifecycle ──────────────────────────────────────────────────────

TEST(RendererTest, StartAndStop) {
    Grid     g{5, 5};
    RvcState s{1, 1, Direction::North};
    std::mutex mtx;

    Renderer r{g, s, mtx, std::chrono::milliseconds{30}};
    r.start();
    std::this_thread::sleep_for(std::chrono::milliseconds{90});
    r.stop();
    // If we reach here without hanging or crashing the test passes.
    SUCCEED();
}
