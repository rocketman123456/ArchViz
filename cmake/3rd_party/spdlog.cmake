option(SPDLOG_BUILD_ALL "Build all artifacts" OFF)

# build shared option
option(SPDLOG_BUILD_SHARED "Build shared library" OFF)

# precompiled headers option
option(SPDLOG_ENABLE_PCH "Build static or shared library using precompiled header to speed up compilation time" OFF)

# build position independent code
option(SPDLOG_BUILD_PIC "Build position independent code (-fPIC)" OFF)

# example options
option(SPDLOG_BUILD_EXAMPLE "Build example" OFF)
option(SPDLOG_BUILD_EXAMPLE_HO "Build header only example" OFF)

# testing options
option(SPDLOG_BUILD_TESTS "Build tests" OFF)
option(SPDLOG_BUILD_TESTS_HO "Build tests using the header only version" OFF)

# bench options
option(SPDLOG_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

# sanitizer options
option(SPDLOG_SANITIZE_ADDRESS "Enable address sanitizer in tests" OFF)

# warning options
option(SPDLOG_BUILD_WARNINGS "Enable compiler warnings" OFF)

# install options
option(SPDLOG_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(SPDLOG_INSTALL "Generate the install target" OFF)
option(SPDLOG_USE_STD_FORMAT "Use std::format instead of fmt library." OFF)
option(SPDLOG_FMT_EXTERNAL "Use external fmt library instead of bundled" OFF)
option(SPDLOG_FMT_EXTERNAL_HO "Use external fmt header-only library instead of bundled" OFF)
option(SPDLOG_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any spdlog exceptions" OFF)

add_subdirectory(spdlog)
