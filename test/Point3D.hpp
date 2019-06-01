#pragma once

#include <tuple>

using Point3D = std::tuple<int, int, int>;

auto const MakePoint3D = [](int x, int y, int z) { return std::make_tuple(x, y, z); };
