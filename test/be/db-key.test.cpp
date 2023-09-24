#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;
using namespace mcfile::be;

TEST_CASE("be/db-key") {
    auto parsed = DbKey::Parse("map_947921552");
    CHECK(!parsed.fIsTagged);
    CHECK(parsed.fUnTagged == "map_947921552");
}
