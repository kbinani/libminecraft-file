#pragma once

namespace mcfile {

class BiomePalette : public PaletteList<biomes::BiomeId, uint16_t, 64> {};

} // namespace mcfile
