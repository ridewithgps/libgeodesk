// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once
#include <cstdlib>
#include <filesystem>
#include <catch2/catch_test_macros.hpp>

namespace geodesk::test {

/**
 * Returns the base path for test data files.
 *
 * Checks (in order):
 * 1. GEODESK_TEST_DATA_PATH environment variable
 * 2. Falls back to ./test-data relative to current working directory
 */
inline std::filesystem::path getTestDataPath()
{
    if (const char* env = std::getenv("GEODESK_TEST_DATA_PATH"))
    {
        return std::filesystem::path(env);
    }
    // Fallback: check relative to current directory
    return std::filesystem::current_path() / "test-data";
}

/**
 * Returns the full path to a test file.
 */
inline std::filesystem::path getTestFile(const std::string& filename)
{
    return getTestDataPath() / filename;
}

/**
 * Checks whether a test data file exists.
 */
inline bool testFileExists(const std::string& filename)
{
    return std::filesystem::exists(getTestFile(filename));
}

} // namespace geodesk::test

/**
 * Macro to skip a test if the required test data file doesn't exist.
 * Must be called at the start of a TEST_CASE (not in a fixture constructor).
 */
#define SKIP_IF_NO_TEST_FILE(filename) \
    if (!geodesk::test::testFileExists(filename)) { \
        SKIP("Test data file not found: " << filename << \
             " (set GEODESK_TEST_DATA_PATH env var)"); \
    }
