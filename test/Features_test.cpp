// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string_view>
#include <catch2/catch_test_macros.hpp>
#include <geodesk/geodesk.h>
#include "TestPaths.h"

using namespace geodesk;
using namespace geodesk::test;


static Node asNode(Feature f)
{
	return f;
}


TEST_CASE("Features")
{
	SKIP_IF_NO_TEST_FILE("world.gol");
	Features world(getTestFile("world.gol").string().c_str());

	Feature france = world("a[boundary=administrative][admin_level=2][name=France]").one();
	Feature paris = world("a[boundary=administrative][admin_level=8][name=Paris]")(france).one();
	std::cout << "Population of Paris: " << paris["population"] << std::endl;
	REQUIRE(paris["name"] == "Paris");
	REQUIRE(paris["population"] > 2'000'000);

	Ways streets = world("[highway=primary]");
	std::cout << "There are " << streets.within(paris).count() << " streets" << std::endl;
	for (Way street : streets.within(paris))
	{
		std::cout << street["name"] << std::endl;
	}
}

TEST_CASE("Features2")
{
	SKIP_IF_NO_TEST_FILE("world.gol");
	Features world(getTestFile("world.gol").string().c_str());

	Feature usa = world("a[boundary=administrative][admin_level=2][name='United States']").one();
	Features buildings = world("a[building]");
	Features usaBuildings = buildings(usa);
	for(int i=0; i<10; i++)
	{
		std::cout << usaBuildings.count() << " buildings" << std::endl;
	}
}


TEST_CASE("Features 3")
{
	SKIP_IF_NO_TEST_FILE("france.gol");
	Features france(getTestFile("france.gol").string().c_str());

	Feature paris = france("a[boundary=administrative][admin_level=8][name=Paris]").one();
	Features museums = france("na[tourism=museum]");
	Features subwayStops = france("n[railway=station][station=subway]");
	for(auto museum: museums(paris))
	{
		std::cout << museum["name"] << std::endl;
		for(auto stop: subwayStops.maxMetersFrom(500, museum.centroid()))
		{
			std::cout << "- " << stop["name"] << std::endl;
		}
	}
}

TEST_CASE("String values")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	std::vector<std::string> l;

	for (auto f : monaco)
	{
		for (auto tag : f.tags())
		{
			std::string s = f.toString() + ": " + static_cast<std::string>(tag.value());
			l.push_back(s);
		}
	}

	std::sort(l.begin(), l.end());

	// Write to file (UTF-8)
	auto outPath = getTestDataPath() / "monaco-cpp.txt";
	std::ofstream out(outPath, std::ios::out | std::ios::trunc);
	out.imbue(std::locale::classic());
	for (const std::string& s : l)
	{
		out << s << "\n";
	}
}


TEST_CASE("Int values")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	std::vector<std::string> l;

	for (auto f : monaco)
	{
		for (auto tag : f.tags())
		{
			int64_t v = tag.value();
			std::string s = f.toString() + ": " + std::to_string(v);
			l.push_back(s);
		}
	}

	std::sort(l.begin(), l.end());

	// Write to file (UTF-8)
	auto outPath = getTestDataPath() / "monaco-ints-cpp.txt";
	std::ofstream out(outPath, std::ios::out | std::ios::trunc);
	out.imbue(std::locale::classic());
	for (const std::string& s : l)
	{
		out << s << "\n";
	}
}


TEST_CASE("Type safety of Features")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	Ways ways = monaco;
	for(Feature f: ways)
	{
		REQUIRE_THROWS_AS(asNode(f), std::runtime_error);
	}
}

TEST_CASE("Empty Features")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	Features set = monaco("na[xyz:nonsense_tag]");
	REQUIRE(!set);
}

TEST_CASE("Lookup with empty Key")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	Key empty;
	TagValue v = monaco.first().value()[empty];
	REQUIRE(v == "");
}

TEST_CASE("Lookup with Key")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	Key highway = monaco.key("highway");
	TagValue v = monaco("w[highway]").first().value()[highway];
	REQUIRE(v != "");
}

TEST_CASE("Iterate tags of anonymous nodes")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	int untaggedNodeCount = 0;
	int highwayNodeCount = 0;
	for (Way street : monaco("w[highway]"))
	{
		for (Node node : street.nodes())
		{
			if (node.tags().isEmpty()) untaggedNodeCount++;
			for (Tag tag : node.tags())
			{
				if (tag.key() == "highway") highwayNodeCount++;
			}
		}
	}
	std::cout << untaggedNodeCount << " untagged nodes, "
		<< highwayNodeCount << " highway nodes.";
}

TEST_CASE("Issue 21")
{
	SKIP_IF_NO_TEST_FILE("world.gol");
	Features world(getTestFile("world.gol").string().c_str());

	Box tileBounds = Box::ofWSEN(-10, -10, 10, 10);
	Features tile = world(tileBounds);
	Features features = tile("w");
}

TEST_CASE("WayNodes")
{
	SKIP_IF_NO_TEST_FILE("liguria.gol");
	Features features(getTestFile("liguria.gol").string().c_str());

	uint64_t count = 0;
	for (auto street : features("w[highway]"))
	{
		for(auto node : street.nodes("n"))
		{
			std::cout << street << ": " << node << '\n';
			count++;
		}
		if (count == 10) break;
	}
	std::cout << count << " waynodes\n";
}


TEST_CASE("role() of non-members (Issue 24)")
{
	SKIP_IF_NO_TEST_FILE("monaco.gol");
	Features monaco(getTestFile("monaco.gol").string().c_str());

	for (Way way : monaco.ways())
	{
		if (way.role()) // <-- SEGFAULT (before fix)
		{
			std::cout << way << " as " << way.role() << std::endl;
		}
	}
}

TEST_CASE("WayNodes with IDs (#25)")
{
	SKIP_IF_NO_TEST_FILE("mcu.gol");
	Features features(getTestFile("mcu.gol").string().c_str());

	uint64_t count = 0;
	for (auto street : features("w[highway]"))
	{
		for(auto node : street.nodes())
		{
			std::cout << street << ": " << node << " at "
				<< std::setprecision(10)
				<< node.lon() << "," << node.lat() << '\n';
			count++;
		}
		if (count == 10) break;
	}
	std::cout << count << " waynodes\n";
}

// TODO: Test if parent relation iterator respect types
