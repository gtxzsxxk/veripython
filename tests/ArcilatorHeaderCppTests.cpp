//
// Created by unknown on 2024/12/2.
//
#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <cstdio>

static const std::string arcNewCmd = "python3 ../tests/arcilator-header-cpp.py";
static const std::string arcOldCmd = "python3 ../tests/ahc_old.py";
static const std::string viewDepth = "--view-depth 1";

std::string executeCommand(const std::string &command) {
    std::array<char, 256> buffer{};
    std::string result{};

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += std::string{buffer.data()};
    }
    return result;
}

std::array<std::pair<std::string, double>, 2> doNewOldTest(const std::string &newCmd, const std::string &oldCmd) {
    auto newStart = std::chrono::steady_clock::now();
    auto retNew = executeCommand(newCmd);
    auto newEnd = std::chrono::steady_clock::now();
    auto retOld = executeCommand(oldCmd);
    auto oldEnd = std::chrono::steady_clock::now();

    auto newMs = std::chrono::duration_cast<std::chrono::milliseconds>(newEnd - newStart).count();
    auto oldMs = std::chrono::duration_cast<std::chrono::milliseconds>(oldEnd - newEnd).count();

    return {std::make_pair(retNew, newMs), std::make_pair(retOld, oldMs)};
}

void doTest(const std::string &testFile) {
    auto res = doNewOldTest(arcNewCmd + " " + testFile + " " + viewDepth, arcOldCmd + " " + testFile + " " + viewDepth);
    std::cout << "New method (ms): " << res[0].second << "\n" << "Old method (ms): " << res[1].second << std::endl;
    EXPECT_EQ(res[0].first, res[1].first);
}

TEST(ArcilatorHeaderCppTests, MuxTestHeader) {
    const auto testFile = "../tests/mlir/mux_test.json";
    doTest(testFile);
}

TEST(ArcilatorHeaderCppTests, ArcilatorOfficialTestHeader) {
    const auto testFile = "../tests/mlir/arcilator.json";
    doTest(testFile);
}

TEST(ArcilatorHeaderCppTests, DecoupledGCDHeader) {
    const auto testFile = "../tests/mlir/decoupledGCD.json";
    doTest(testFile);
}

TEST(ArcilatorHeaderCppTests, RISCVMiniHeader) {
    const auto testFile = "../tests/mlir/riscv-mini.json";
    doTest(testFile);
}
