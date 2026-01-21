// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <catch2/catch_test_macros.hpp>
#include <geodesk/match/MatcherCompiler.h>
#include <geodesk/feature/FeatureStore.h>
#include <geodesk/geodesk.h>
#include "../TestPaths.h"

using namespace geodesk;
using namespace geodesk::test;

TEST_CASE("Multiple negative clauses")
{
    SKIP_IF_NO_TEST_FILE("berlin.gol");
    FeatureStore* store = FeatureStore::openSingle(
        getTestFile("berlin.gol").string().c_str());
    MatcherCompiler mc(store);
    const MatcherHolder* matcher = mc.getMatcher(
        "a[amenity=parking][access!=no,private,employees,staff,permit,military,agricultural,restricted,delivery][parking!=carports,half_on_kerb,half_on_shoulder,layby,left,on_kerb,sheds,shoulder,lane,street_side]");
}


TEST_CASE("Issue 29")
{
    SKIP_IF_NO_TEST_FILE("berlin.gol");
    Features berlin(getTestFile("berlin.gol").string().c_str());

    for (auto f : berlin(
        "a[amenity=parking][parking!=street_side]"))
    {
        std::cout << f["parking"] << std::endl;
    }
}

TEST_CASE("Issue 30")
{
    SKIP_IF_NO_TEST_FILE("mcu.gol");
    Features berlin(getTestFile("mcu.gol").string().c_str());

    for (auto f : berlin(
        "a[amenity=parking][parking!=street_side]"))
    {
        std::cout << f["parking"] << std::endl;
    }
}

TEST_CASE("Issue geodesk-py#62")
{
    SKIP_IF_NO_TEST_FILE("de.gol");
    Features world(getTestFile("de.gol").string().c_str());

    for (auto f : world(
        "n[!geodesk:orphan][power]"))
    {
        std::cout << f["parking"] << std::endl;
    }
}


TEST_CASE("Issue 31")
{
    SKIP_IF_NO_TEST_FILE("mcu.gol");
    Features world(getTestFile("mcu.gol").string().c_str());

    for (auto f : world("na[shop],n[amenity]"))
    {
        std::cout << f["parking"] << std::endl;
    }
}

TEST_CASE("Polyform queries")
{
    SKIP_IF_NO_TEST_FILE("mcu.gol");
    Features world(getTestFile("mcu.gol").string().c_str());
    (void)world("na[shop],n[amenity]");
    (void)world("wa[highway],r[local_key_banana]");
    (void)world("r[local_key_apple!=some_value][local_key_cherry],wa[highway],r[local_key_banana],w[!amenity]");

    // ensure uniform queries don't have type ops
    (void)world("na[shop],na[amenity]");
    (void)world("na[local_key_apple]");
}


TEST_CASE("[k][k!=v] queries")
{
    SKIP_IF_NO_TEST_FILE("mcu.gol");
    Features world(getTestFile("mcu.gol").string().c_str());
    (void)world("na[shop][shop!=bakery]");
    (void)world("na[shop][shop!=bakery][sells_bananas]");
    (void)world("na[local_key_banana][local_key_banana!=cherry][shop]");
    (void)world("na[local_key_banana][local_key_banana!=cherry][sells_bananas]");
}

TEST_CASE("Issue 32")
{
    SKIP_IF_NO_TEST_FILE("mcu.gol");
    Features world(getTestFile("mcu.gol").string().c_str());
    (void)world("n,n");
    (void)world("na,n");
}
