cmake_minimum_required(VERSION 4.0)
project(BacktestingEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# 1. Main Application Executable
add_executable(BacktestingEngine main.cpp src/DataLoader.cpp src/Backtester.cpp src/BuyandHold.cpp src/MarketView.cpp src/Portfolio.cpp src/SMA.cpp src/Indicators.cpp)

# 2. Fetch Catch2 v3 framework
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# 3. Enable CTest testing framework
enable_testing()

# 4. Create the test executable
add_executable(unit_tests test.cpp src/MarketView.cpp src/SMA.cpp src/Indicators.cpp src/Portfolio.cpp )

# Link Catch2's automatic main entry point
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

# 5. Automatically register tests with CTest
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(unit_tests)

# ---------------------------------------------------------------------------
# Google Benchmark Setup
# ---------------------------------------------------------------------------
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

# Benchmark Executable
add_executable(benchmarks benchmarks/bench_main.cpp)
target_link_libraries(benchmarks PRIVATE benchmark::benchmark)