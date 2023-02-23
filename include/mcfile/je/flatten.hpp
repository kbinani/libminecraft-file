#pragma once

namespace mcfile::je {
class Flatten {
    Flatten() = delete;

public:
    static std::optional<std::string> Item(int16_t id, int16_t *damage) {
        using namespace std;
        int16_t safety = 0;
        assert(damage);
        if (!damage) {
            damage = &safety;
        }
        if (id < 256) {
            auto block = Block(id, *damage);
            if (block) {
                *damage = 0;
                return string(block->fName);
            }
        }
#define ns "minecraft:"
#define egg "_spawn_egg";
        switch (id) {
        case 256: return ns "iron_shovel";
        case 257: return ns "iron_pickaxe";
        case 258: return ns "iron_axe";
        case 259: return ns "flint_and_steel";
        case 260: return ns "apple";
        case 261: return ns "bow";
        case 262: return ns "arrow";
        case 263:
            switch (DrainDamage(damage)) {
            case 1:
                return ns "charcoal";
            case 0:
            default:
                return ns "coal";
            }
        case 264: return ns "diamond";
        case 265: return ns "iron_ingot";
        case 266: return ns "gold_ingot";
        case 267: return ns "iron_sword";
        case 268: return ns "wooden_sword";
        case 269: return ns "wooden_shovel";
        case 270: return ns "wooden_pickaxe";
        case 271: return ns "wooden_axe";
        case 272: return ns "stone_sword";
        case 273: return ns "stone_shovel";
        case 274: return ns "stone_pickaxe";
        case 275: return ns "stone_axe";
        case 276: return ns "diamond_sword";
        case 277: return ns "diamond_shovel";
        case 278: return ns "diamond_pickaxe";
        case 279: return ns "diamond_axe";
        case 280: return ns "stick";
        case 281: return ns "bowl";
        case 282: return ns "mushroom_stew";
        case 283: return ns "golden_sword";
        case 284: return ns "golden_shovel";
        case 285: return ns "golden_pickaxe";
        case 286: return ns "golden_axe";
        case 287: return ns "string";
        case 288: return ns "feather";
        case 289: return ns "gunpowder";
        case 290: return ns "wooden_hoe";
        case 291: return ns "stone_hoe";
        case 292: return ns "iron_hoe";
        case 293: return ns "diamond_hoe";
        case 294: return ns "golden_hoe";
        case 295: return ns "wheat_seeds";
        case 296: return ns "wheat";
        case 297: return ns "bread";
        case 298: return ns "leather_helmet";
        case 299: return ns "leather_chestplate";
        case 300: return ns "leather_leggings";
        case 301: return ns "leather_boots";
        case 302: return ns "chainmail_helmet";
        case 303: return ns "chainmail_chestplate";
        case 304: return ns "chainmail_leggings";
        case 305: return ns "chainmail_boots";
        case 306: return ns "iron_helmet";
        case 307: return ns "iron_chestplate";
        case 308: return ns "iron_leggings";
        case 309: return ns "iron_boots";
        case 310: return ns "diamond_helmet";
        case 311: return ns "diamond_chestplate";
        case 312: return ns "diamond_leggings";
        case 313: return ns "diamond_boots";
        case 314: return ns "golden_helmet";
        case 315: return ns "golden_chestplate";
        case 316: return ns "golden_leggings";
        case 317: return ns "golden_boots";
        case 318: return ns "flint";
        case 319: return ns "porkchop";
        case 320: return ns "cooked_porkchop";
        case 321: return ns "painting";
        case 322:
            switch (DrainDamage(damage)) {
            case 1:
                return ns "enchanted_golden_apple";
            case 0:
            default:
                return ns "golden_apple";
            }
        case 323: return ns "sign";
        case 324: return ns "oak_door";
        case 325: return ns "bucket";
        case 326: return ns "water_bucket";
        case 327: return ns "lava_bucket";
        case 328: return ns "minecart";
        case 329: return ns "saddle";
        case 330: return ns "iron_door";
        case 331: return ns "redstone";
        case 332: return ns "snowball";
        case 333: return ns "oak_boat";
        case 334: return ns "leather";
        case 335: return ns "milk_bucket";
        case 336: return ns "brick";
        case 337: return ns "clay_ball";
        case 338: return ns "sugar_cane";
        case 339: return ns "paper";
        case 340: return ns "book";
        case 341: return ns "slime_ball";
        case 342: return ns "chest_minecart";
        case 343: return ns "furnace_minecart";
        case 344: return ns "egg";
        case 345: return ns "compass";
        case 346: return ns "fishing_rod";
        case 347: return ns "clock";
        case 348: return ns "glowstone_dust";
        case 349:
            switch (DrainDamage(damage)) {
            case 1:
                return ns "salmon";
            case 2:
                return ns "tropical_fish";
            case 3:
                return ns "pufferfish";
            case 0:
            default:
                return ns "cod";
            }
        case 350:
            switch (DrainDamage(damage)) {
            case 1:
                return ns "cooked_salmon";
            case 0:
            default:
                return ns "cooked_fish";
            }
        case 351:
            switch (DrainDamage(damage)) {
            case 1:
                return ns "red_dye";
            case 2:
                return ns "green_dye";
            case 3:
                return ns "cocoa_beans";
            case 4:
                return ns "lapis_lazuli";
            case 5:
                return ns "purple_dye";
            case 6:
                return ns "cyan_dye";
            case 7:
                return ns "light_gray_dye";
            case 8:
                return ns "gray_dye";
            case 9:
                return ns "pink_dye";
            case 10:
                return ns "lime_dye";
            case 11:
                return ns "yellow_dye";
            case 12:
                return ns "light_blue_dye";
            case 13:
                return ns "magenta_dye";
            case 14:
                return ns "orange_dye";
            case 15:
                return ns "bone_meal";
            case 0:
            default:
                return ns "ink_sac";
            }
        case 352: return ns "bone";
        case 353: return ns "sugar";
        case 354: return ns "cake";
        case 355: return ns "red_bed";
        case 356: return ns "repeater";
        case 357: return ns "cookie";
        case 359: return ns "shears";
        case 360: return ns "melon_slice";
        case 361: return ns "pumpkin_seeds";
        case 362: return ns "melon_seeds";
        case 363: return ns "beef";
        case 364: return ns "cooked_beef";
        case 365: return ns "chicken";
        case 366: return ns "cooked_chicken";
        case 367: return ns "rotten_flesh";
        case 368: return ns "ender_pearl";
        case 369: return ns "blaze_rod";
        case 370: return ns "ghast_tear";
        case 371: return ns "gold_nugget";
        case 372: return ns "nether_wart";
        case 373: return ns "potion";
        case 374: return ns "glass_bottle";
        case 375: return ns "spider_eye";
        case 376: return ns "fermented_spider_eye";
        case 377: return ns "blaze_powder";
        case 378: return ns "magma_cream";
        case 379: return ns "brewing_stand";
        case 380: return ns "cauldron";
        case 381: return ns "ender_eye";
        case 382: return ns "glistering_melon_slice";
        case 383:
            switch (DrainDamage(damage)) {
            case 50:
                return ns "creeper" egg;
            case 51:
                return ns "skeleton" egg;
            case 52:
                return ns "spider" egg;
            case 54:
                return ns "zombie" egg;
            case 55:
                return ns "slime" egg;
            case 56:
                return ns "ghast" egg;
            case 57:
                return ns "zombified_piglin" egg;
            case 58:
                return ns "enderman" egg;
            case 59:
                return ns "cave_spider" egg;
            case 60:
                return ns "silverfish" egg;
            case 61:
                return ns "blaze" egg;
            case 62:
                return ns "magma_cube" egg;
            case 65:
                return ns "bat" egg;
            case 66:
                return ns "witch" egg;
            case 90:
                return ns "pig" egg;
            case 91:
                return ns "sheep" egg;
            case 92:
                return ns "cow" egg;
            case 93:
                return ns "chicken" egg;
            case 94:
                return ns "squid" egg;
            case 95:
                return ns "wolf" egg;
            case 96:
                return ns "mooshroom" egg;
            case 98:
                return ns "ocelot" egg;
            case 100:
                return ns "horse" egg;
            case 120:
                return ns "villager" egg;
            default:
                return nullopt;
            }
        case 384: return ns "experience_bottle";
        case 385: return ns "fire_charge";
        case 386: return ns "writable_book";
        case 388: return ns "emerald";
        case 389: return ns "item_frame";
        case 390: return ns "flower_pot";
        case 391: return ns "carrot";
        case 392: return ns "potato";
        case 393: return ns "baked_potato";
        case 394: return ns "poisonous_potato";
        case 395: return ns "map";
        case 396: return ns "golden_carrot";
        case 397:
            switch (DrainDamage(damage)) {
            case 1:
                return ns "wither_skeleton_skull";
            case 2:
                return ns "zombie_head";
            case 3:
                return ns "player_head";
            case 4:
                return ns "creeper_head";
            case 0:
            default:
                return ns "skeleton_skull";
            }
        case 398: return ns "carrot_on_a_stick";
        case 399: return ns "nether_star";
        case 400: return ns "pumpkin_pie";
        case 402: return ns "firework_star";
        case 403: return ns "enchanted_book";
        case 404: return ns "comparator";
        case 405: return ns "nether_brick";
        case 406: return ns "quartz";
        case 407: return ns "tnt_minecart";
        case 408: return ns "hopper_minecart";
        case 417: return ns "iron_horse_armor";
        case 418: return ns "golden_horse_armor";
        case 419: return ns "diamond_horse_armor";
        case 420: return ns "lead";
        case 421: return ns "name_tag";
        case 2256: return ns "music_disc_13";
        case 2257: return ns "music_disc_cat";
        case 2258: return ns "music_disc_blocks";
        case 2259: return ns "music_disc_chirp";
        case 2260: return ns "music_disc_far";
        case 2261: return ns "music_disc_mall";
        case 2262: return ns "music_disc_mellohi";
        case 2267: return ns "music_disc_wait";
        case 2263: return ns "music_disc_stal";
        case 2264: return ns "music_disc_strad";
        case 2265: return ns "music_disc_ward";
        case 2266: return ns "music_disc_11";
        }
        return nullopt;
#undef ns
#undef egg
    }

    static std::shared_ptr<mcfile::je::Block const> Block(uint16_t blockId, uint8_t data) {
        blocks::BlockId id = blocks::minecraft::air;
        std::map<std::string, std::string> props;
        switch (blockId) {
        case 0: id = blocks::minecraft::air; break;
        case 1:
            switch (data) {
            case 1: id = blocks::minecraft::granite; break;
            case 2: id = blocks::minecraft::polished_granite; break;
            case 3: id = blocks::minecraft::diorite; break;
            case 4: id = blocks::minecraft::polished_diorite; break;
            case 5: id = blocks::minecraft::andesite; break;
            case 6: id = blocks::minecraft::polished_andesite; break;
            case 0:
            default:
                id = blocks::minecraft::stone;
                break;
            }
            break;
        case 2: id = blocks::minecraft::grass_block; break;
        case 3:
            switch (data) {
            case 1: id = blocks::minecraft::coarse_dirt; break;
            case 2: id = blocks::minecraft::podzol; break;
            case 0:
            default:
                id = blocks::minecraft::dirt;
                break;
            }
            break;
        case 4: id = blocks::minecraft::cobblestone; break;
        case 5:
            switch (data) {
            case 1: id = blocks::minecraft::spruce_planks; break;
            case 2: id = blocks::minecraft::birch_planks; break;
            case 3: id = blocks::minecraft::jungle_planks; break;
            case 4: id = blocks::minecraft::acacia_planks; break;
            case 5: id = blocks::minecraft::dark_oak_planks; break;
            case 0:
            default:
                id = blocks::minecraft::oak_planks;
                break;
            }
            break;
        case 6:
            switch (data & 0x7) {
            case 1: id = blocks::minecraft::spruce_sapling; break;
            case 2: id = blocks::minecraft::birch_sapling; break;
            case 3: id = blocks::minecraft::jungle_sapling; break;
            case 4: id = blocks::minecraft::acacia_sapling; break;
            case 5: id = blocks::minecraft::dark_oak_sapling; break;
            case 0:
            default:
                id = blocks::minecraft::oak_sapling;
                break;
            }
            break;
        case 7: id = blocks::minecraft::bedrock; break;
        case 8: id = blocks::minecraft::water; break; //TODO: flowing_water
        case 9:
            id = blocks::minecraft::water;
            props["level"] = std::to_string(data);
            break;
        case 10: id = blocks::minecraft::lava; break; //TODO: flowing_lava
        case 11:
            id = blocks::minecraft::lava;
            props["level"] = std::to_string(data);
            break;
        case 12:
            switch (data) {
            case 1: id = blocks::minecraft::red_sand; break;
            case 0:
            default:
                id = blocks::minecraft::sand;
            }
            break;
        case 13: id = blocks::minecraft::gravel; break;
        case 14: id = blocks::minecraft::gold_ore; break;
        case 15: id = blocks::minecraft::iron_ore; break;
        case 16: id = blocks::minecraft::coal_ore; break;
        case 17:
            switch (data & 0x3) {
            case 1: id = blocks::minecraft::spruce_log; break;
            case 2: id = blocks::minecraft::birch_log; break;
            case 3: id = blocks::minecraft::jungle_log; break;
            case 0:
            default:
                id = blocks::minecraft::oak_log;
                break;
            }
            Log(data, props);
            break;
        case 18:
            switch (data & 0x3) {
            case 1:
                id = blocks::minecraft::spruce_leaves;
                break;
            case 2:
                id = blocks::minecraft::birch_leaves;
                break;
            case 3:
                id = blocks::minecraft::jungle_leaves;
                break;
            case 0:
            default:
                id = blocks::minecraft::oak_leaves;
                break;
            }
            Leaves(data, props);
            break;
        case 19:
            switch (data) {
            case 1: id = blocks::minecraft::wet_sponge; break;
            case 0:
            default:
                id = blocks::minecraft::sponge;
                break;
            }
            break;
        case 20: id = blocks::minecraft::glass; break;
        case 21: id = blocks::minecraft::lapis_ore; break;
        case 22: id = blocks::minecraft::lapis_block; break;
        case 23:
            id = blocks::minecraft::dispenser;
            props["facing"] = FacingA(data);
            break;
        case 24:
            switch (data) {
            case 1: id = blocks::minecraft::chiseled_sandstone; break;
            case 2: id = blocks::minecraft::cut_sandstone; break;
            case 0:
            default:
                id = blocks::minecraft::sandstone;
                break;
            }
            break;
        case 25: id = blocks::minecraft::note_block; break;
        case 26:
            id = blocks::minecraft::red_bed;
            Bed(data, props);
            break;
        case 27:
            id = blocks::minecraft::powered_rail;
            PoweredRail(data, props);
            break;
        case 28:
            id = blocks::minecraft::detector_rail;
            PoweredRail(data, props);
            break;
        case 29:
            id = blocks::minecraft::sticky_piston;
            Piston(data, props);
            break;
        case 30: id = blocks::minecraft::cobweb; break;
        case 31:
            switch (data) {
            case 2: id = blocks::minecraft::fern; break;
            case 1:
            case 0: // shrub. Legacy console edition only
            default:
                id = blocks::minecraft::grass;
                break;
            }
            break;
        case 32: id = blocks::minecraft::dead_bush; break;
        case 33:
            id = blocks::minecraft::piston;
            Piston(data, props);
            break;
        case 34:
            id = blocks::minecraft::piston_head;
            props["facing"] = FacingA(data & 7);
            props["type"] = (data & 0x8) == 0x8 ? "sticky" : "normal";
            break;
        case 35:
            switch (data) {
            case 1: id = blocks::minecraft::orange_wool; break;
            case 2: id = blocks::minecraft::magenta_wool; break;
            case 3: id = blocks::minecraft::light_blue_wool; break;
            case 4: id = blocks::minecraft::yellow_wool; break;
            case 5: id = blocks::minecraft::lime_wool; break;
            case 6: id = blocks::minecraft::pink_wool; break;
            case 7: id = blocks::minecraft::gray_wool; break;
            case 8: id = blocks::minecraft::light_gray_wool; break;
            case 9: id = blocks::minecraft::cyan_wool; break;
            case 10: id = blocks::minecraft::purple_wool; break;
            case 11: id = blocks::minecraft::blue_wool; break;
            case 12: id = blocks::minecraft::brown_wool; break;
            case 13: id = blocks::minecraft::green_wool; break;
            case 14: id = blocks::minecraft::red_wool; break;
            case 15: id = blocks::minecraft::black_wool; break;
            case 0:
            default:
                id = blocks::minecraft::white_wool;
                break;
            }
            break;
        case 37: id = blocks::minecraft::dandelion; break;
        case 38:
            switch (data) {
            case 0:
                id = blocks::minecraft::poppy;
                break;
            case 1:
                id = blocks::minecraft::blue_orchid;
                break;
            case 2:
                id = blocks::minecraft::allium;
                break;
            case 3:
                id = blocks::minecraft::azure_bluet;
                break;
            case 4:
                id = blocks::minecraft::red_tulip;
                break;
            case 5:
                id = blocks::minecraft::orange_tulip;
                break;
            case 6:
                id = blocks::minecraft::white_tulip;
                break;
            case 7:
                id = blocks::minecraft::pink_tulip;
                break;
            case 8:
                id = blocks::minecraft::oxeye_daisy;
                break;
            }
            break;
        case 39: id = blocks::minecraft::brown_mushroom; break;
        case 40: id = blocks::minecraft::red_mushroom; break;
        case 41: id = blocks::minecraft::gold_block; break;
        case 42: id = blocks::minecraft::iron_block; break;
        case 43:
            switch (data) {
            case 1: id = blocks::minecraft::sandstone_slab; break;
            case 2: id = blocks::minecraft::oak_slab; break;
            case 3: id = blocks::minecraft::cobblestone_slab; break;
            case 4: id = blocks::minecraft::brick_slab; break;
            case 5: id = blocks::minecraft::stone_brick_slab; break;
            case 6: id = blocks::minecraft::nether_brick_slab; break;
            case 7: id = blocks::minecraft::quartz_slab; break;
            case 0:
            default:
                id = blocks::minecraft::smooth_stone_slab;
                break;
            }
            props["type"] = "double";
            break;
        case 44:
            switch (data & 0x7) {
            case 1:
                id = blocks::minecraft::sandstone_slab;
                break;
            case 2:
                id = blocks::minecraft::oak_slab;
                break;
            case 3:
                id = blocks::minecraft::cobblestone_slab;
                break;
            case 4:
                id = blocks::minecraft::brick_slab;
                break;
            case 5:
                id = blocks::minecraft::stone_brick_slab;
                break;
            case 6:
                id = blocks::minecraft::nether_brick_slab;
                break;
            case 7:
                id = blocks::minecraft::quartz_slab;
                break;
            case 0:
            default:
                id = id = blocks::minecraft::smooth_stone_slab;
                break;
            }
            props["type"] = ((data >> 3) & 0x1) == 0x1 ? "top" : "bottom";
            break;
        case 45: id = blocks::minecraft::bricks; break;
        case 46: id = blocks::minecraft::tnt; break;
        case 47: id = blocks::minecraft::bookshelf; break;
        case 48: id = blocks::minecraft::mossy_cobblestone; break;
        case 49: id = blocks::minecraft::obsidian; break;
        case 50:
            if (data < 5) {
                static std::string const facing[4] = {"east", "west", "south", "north"};
                props["facing"] = facing[std::clamp<uint8_t>(data, 1, 4) - 1];
                id = blocks::minecraft::wall_torch;
            } else {
                id = blocks::minecraft::torch;
            }
            break;
        case 51: id = blocks::minecraft::fire; break;
        case 52: id = blocks::minecraft::spawner; break;
        case 53:
            id = blocks::minecraft::oak_stairs;
            Stairs(data, props);
            break;
        case 54:
            id = blocks::minecraft::chest;
            Chest(data, props);
            break;
        case 55:
            id = blocks::minecraft::redstone_wire;
            props["power"] = std::to_string(data);
            break;
        case 56: id = blocks::minecraft::diamond_ore; break;
        case 57: id = blocks::minecraft::diamond_block; break;
        case 58: id = blocks::minecraft::crafting_table; break;
        case 59:
            id = blocks::minecraft::wheat;
            props["age"] = std::to_string(data);
            break;
        case 60: id = blocks::minecraft::farmland; break;
        case 61:
        case 62: // lit
            id = blocks::minecraft::furnace;
            switch (data) {
            case 3: props["facing"] = "south"; break;
            case 4: props["facing"] = "west"; break;
            case 5: props["facing"] = "east"; break;
            case 2:
            default:
                props["facing"] = "north";
                break;
            }
            break;
        case 63:
            id = blocks::minecraft::oak_sign;
            props["rotation"] = std::to_string(data);
            break;
        case 64:
            id = blocks::minecraft::oak_door;
            Door(data, props);
            break;
        case 65:
            id = blocks::minecraft::ladder;
            switch (data) {
            case 5: props["facing"] = "east"; break;
            case 3: props["facing"] = "south"; break;
            case 4: props["facing"] = "west"; break;
            case 2:
            default:
                props["facing"] = "north";
                break;
            }
            break;
        case 66:
            id = blocks::minecraft::rail;
            Rail(data, props);
            break;
        case 67:
            id = blocks::minecraft::cobblestone_stairs;
            Stairs(data, props);
            break;
        case 68:
            id = blocks::minecraft::oak_wall_sign;
            switch (data) {
            case 3:
                props["facing"] = "south";
                break;
            case 4:
                props["facing"] = "west";
                break;
            case 5:
                props["facing"] = "east";
                break;
            case 2:
            default:
                props["facing"] = "north";
            }
            break;
        case 69:
            id = blocks::minecraft::lever;
            Lever(data, props);
            break;
        case 70: id = blocks::minecraft::stone_pressure_plate; break;
        case 71:
            id = blocks::minecraft::iron_door;
            Door(data, props);
            break;
        case 72: id = blocks::minecraft::oak_pressure_plate; break;
        case 73: id = blocks::minecraft::redstone_ore; break;
        case 74: id = blocks::minecraft::redstone_ore; break; // glowing_redstone_ore
        case 75:
        case 76:
            if (data == 5) {
                id = blocks::minecraft::redstone_torch;
            } else {
                id = blocks::minecraft::redstone_wall_torch;
                switch (data) {
                case 1:
                    props["facing"] = "east";
                    break;
                case 2:
                    props["facing"] = "west";
                    break;
                case 3:
                    props["facing"] = "south";
                    break;
                case 4:
                    props["facing"] = "north";
                    break;
                }
            }
            props["lit"] = blockId == 76 ? "true" : "false";
            break;
        case 77:
            id = blocks::minecraft::stone_button;
            Button(data, props);
            break;
        case 78:
            id = blocks::minecraft::snow;
            props["layers"] = std::to_string(std::clamp(data + 1, 1, 8));
            break;
        case 79: id = blocks::minecraft::ice; break;
        case 80: id = blocks::minecraft::snow_block; break;
        case 81: id = blocks::minecraft::cactus; break;
        case 82: id = blocks::minecraft::clay; break;
        case 83: id = blocks::minecraft::sugar_cane; break;
        case 84: id = blocks::minecraft::jukebox; break;
        case 85: id = blocks::minecraft::oak_fence; break;
        case 86:
            id = blocks::minecraft::carved_pumpkin;
            props["facing"] = FacingB(data);
            break;
        case 87: id = blocks::minecraft::netherrack; break;
        case 88: id = blocks::minecraft::soul_sand; break;
        case 89: id = blocks::minecraft::glowstone; break;
        case 90:
            id = blocks::minecraft::nether_portal;
            switch (data) {
            case 2:
                props["axis"] = "z";
                break;
            case 1:
            default:
                props["axis"] = "x";
                break;
            }
            break;
        case 91:
            id = blocks::minecraft::jack_o_lantern;
            props["facing"] = FacingB(data);
            break;
        case 92: id = blocks::minecraft::cake; break;
        case 93:
        case 94:
            id = blocks::minecraft::repeater;
            switch (data & 0x3) {
            case 1:
                props["facing"] = "west";
                break;
            case 2:
                props["facing"] = "north";
                break;
            case 3:
                props["facing"] = "east";
                break;
            case 0:
            default:
                props["facing"] = "south";
                break;
            }
            props["delay"] = std::to_string(((data & 0xc) >> 2) + 1);
            props["powered"] = blockId == 94 ? "true" : "false";
            break;
        case 95:
            switch (data) {
            case 1: id = blocks::minecraft::orange_stained_glass; break;
            case 2: id = blocks::minecraft::magenta_stained_glass; break;
            case 3: id = blocks::minecraft::light_blue_stained_glass; break;
            case 4: id = blocks::minecraft::yellow_stained_glass; break;
            case 5: id = blocks::minecraft::lime_stained_glass; break;
            case 6: id = blocks::minecraft::pink_stained_glass; break;
            case 7: id = blocks::minecraft::gray_stained_glass; break;
            case 8: id = blocks::minecraft::light_gray_stained_glass; break;
            case 9: id = blocks::minecraft::cyan_stained_glass; break;
            case 10: id = blocks::minecraft::purple_stained_glass; break;
            case 11: id = blocks::minecraft::blue_stained_glass; break;
            case 12: id = blocks::minecraft::brown_stained_glass; break;
            case 13: id = blocks::minecraft::green_stained_glass; break;
            case 14: id = blocks::minecraft::red_stained_glass; break;
            case 15: id = blocks::minecraft::black_stained_glass; break;
            case 0:
            default:
                id = blocks::minecraft::white_stained_glass;
                break;
            }
            break;
        case 96:
            id = blocks::minecraft::oak_trapdoor;
            Trapdoor(data, props);
            break;
        case 97:
            switch (data) {
            case 1: id = blocks::minecraft::infested_cobblestone; break;
            case 2: id = blocks::minecraft::infested_stone_bricks; break;
            case 3: id = blocks::minecraft::infested_mossy_stone_bricks; break;
            case 4: id = blocks::minecraft::infested_cracked_stone_bricks; break;
            case 5: id = blocks::minecraft::infested_chiseled_stone_bricks; break;
            case 0:
            default:
                id = blocks::minecraft::infested_stone;
                break;
            }
            break;
        case 98:
            switch (data) {
            case 1: id = blocks::minecraft::mossy_stone_bricks; break;
            case 2: id = blocks::minecraft::cracked_stone_bricks; break;
            case 3: id = blocks::minecraft::chiseled_stone_bricks; break;
            case 0:
            default:
                id = blocks::minecraft::stone_bricks;
                break;
            }
            break;
        case 99:
            id = MushroomBlock(blocks::minecraft::brown_mushroom_block, data, props);
            break;
        case 100:
            id = MushroomBlock(blocks::minecraft::red_mushroom_block, data, props);
            break;
        case 101: id = blocks::minecraft::iron_bars; break;
        case 102: id = blocks::minecraft::glass_pane; break;
        case 103: id = blocks::minecraft::melon; break;
        case 104:
            id = blocks::minecraft::pumpkin_stem;
            props["age"] = std::to_string(data);
            break;
        case 105:
            id = blocks::minecraft::melon_stem;
            props["age"] = std::to_string(data);
            break;
        case 106:
            id = blocks::minecraft::vine;
            props["south"] = (data & 0x1) == 0x1 ? "true" : "false";
            props["west"] = (data & 0x2) == 0x2 ? "true" : "false";
            props["north"] = (data & 0x4) == 0x4 ? "true" : "false";
            props["east"] = (data & 0x8) == 0x8 ? "true" : "false";
            break;
        case 107: id = blocks::minecraft::oak_fence_gate; break;
        case 108:
            id = blocks::minecraft::brick_stairs;
            Stairs(data, props);
            break;
        case 109:
            id = blocks::minecraft::stone_brick_stairs;
            Stairs(data, props);
            break;
        case 110: id = blocks::minecraft::mycelium; break;
        case 111: id = blocks::minecraft::lily_pad; break;
        case 112: id = blocks::minecraft::nether_bricks; break;
        case 113: id = blocks::minecraft::nether_brick_fence; break;
        case 114:
            id = blocks::minecraft::nether_brick_stairs;
            Stairs(data, props);
            break;
        case 115:
            id = blocks::minecraft::nether_wart;
            props["age"] = std::to_string(data);
            break;
        case 116: id = blocks::minecraft::enchanting_table; break;
        case 117: id = blocks::minecraft::brewing_stand; break;
        case 118:
            if (data / 2 == 0) {
                id = blocks::minecraft::cauldron;
            } else {
                id = blocks::minecraft::water_cauldron;
                props["level"] = std::to_string(data / 2);
            }
            break;
        case 119: id = blocks::minecraft::end_portal; break;
        case 120:
            id = blocks::minecraft::end_portal_frame;
            switch (data & 0x3) {
            case 1: props["facing"] = "west"; break;
            case 2: props["facing"] = "north"; break;
            case 3: props["facing"] = "east"; break;
            case 0:
            default:
                props["facing"] = "south";
                break;
            }
            props["eye"] = data > 3 ? "true" : "false";
            break;
        case 121: id = blocks::minecraft::end_stone; break;
        case 122: id = blocks::minecraft::dragon_egg; break;
        case 123:
            id = blocks::minecraft::redstone_lamp;
            props["lit"] = "false";
            break;
        case 124:
            id = blocks::minecraft::redstone_lamp;
            props["lit"] = "true";
            break;
        case 125:
            switch (data) {
            case 1: id = blocks::minecraft::spruce_slab; break;
            case 2: id = blocks::minecraft::birch_slab; break;
            case 3: id = blocks::minecraft::jungle_slab; break;
            case 4: id = blocks::minecraft::acacia_slab; break;
            case 5: id = blocks::minecraft::dark_oak_slab; break;
            case 0:
            default:
                id = blocks::minecraft::oak_slab;
                break;
            }
            props["type"] = "double";
            break;
        case 126:
            switch (data & 0x7) {
            case 1: id = blocks::minecraft::spruce_slab; break;
            case 2: id = blocks::minecraft::birch_slab; break;
            case 3: id = blocks::minecraft::jungle_slab; break;
            case 4: id = blocks::minecraft::acacia_slab; break;
            case 5: id = blocks::minecraft::dark_oak_slab; break;
            case 0:
            default:
                id = blocks::minecraft::oak_slab;
                break;
            }
            props["type"] = data < 8 ? "bottom" : "top";
            break;
        case 127:
            id = blocks::minecraft::cocoa;
            props["facing"] = FacingN2E3S0W1(data & 0x3);
            props["age"] = std::to_string(data >> 2);
            break;
        case 128:
            id = blocks::minecraft::sandstone_stairs;
            Stairs(data, props);
            break;
        case 129: id = blocks::minecraft::emerald_ore; break;
        case 130:
            id = blocks::minecraft::ender_chest;
            Chest(data, props);
            break;
        case 131:
            id = blocks::minecraft::tripwire_hook;
            switch (data & 0x3) {
            case 1:
                props["facing"] = "west";
                break;
            case 2:
                props["facing"] = "north";
                break;
            case 3:
                props["facing"] = "east";
                break;
            case 0:
            default:
                props["facing"] = "south";
                break;
            }
            props["attached"] = (data & 0x4) == 0x4 ? "true" : "false";
            props["powered"] = (data & 0x8) == 0x8 ? "true" : "false";
            break;
        case 132: id = blocks::minecraft::tripwire; break;
        case 133: id = blocks::minecraft::emerald_block; break;
        case 134:
            id = blocks::minecraft::spruce_stairs;
            Stairs(data, props);
            break;
        case 135:
            id = blocks::minecraft::birch_stairs;
            Stairs(data, props);
            break;
        case 136:
            id = blocks::minecraft::jungle_stairs;
            Stairs(data, props);
            break;
        case 137: id = blocks::minecraft::command_block; break;
        case 138: id = blocks::minecraft::beacon; break;
        case 139:
            switch (data) {
            case 1: id = blocks::minecraft::mossy_cobblestone_wall; break;
            case 0:
            default:
                id = blocks::minecraft::cobblestone_wall;
            }
            break;
        case 140: id = blocks::minecraft::flower_pot; break;
        case 141:
            id = blocks::minecraft::carrots;
            props["age"] = std::to_string(data);
            break;
        case 142:
            id = blocks::minecraft::potatoes;
            props["age"] = std::to_string(data);
            break;
        case 143:
            id = blocks::minecraft::oak_button;
            Button(data, props);
            break;
        case 144:
            switch (data) {
            case 1: id = blocks::minecraft::skeleton_skull; break;
            case 3:
                id = blocks::minecraft::skeleton_wall_skull;
                props["facing"] = "south";
                break;
            case 4:
                id = blocks::minecraft::skeleton_wall_skull;
                props["facing"] = "west";
                break;
            case 2:
                id = blocks::minecraft::skeleton_wall_skull;
                props["facing"] = "north";
                break;
            case 5:
                id = blocks::minecraft::skeleton_wall_skull;
                props["facing"] = "east";
                break;
            }
            break;
        case 145:
            switch (data & 0x3) {
            case 3: props["facing"] = "east"; break;
            case 1: props["facing"] = "west"; break;
            case 2: props["facing"] = "north"; break;
            case 0:
            default:
                props["facing"] = "south";
                break;
            }
            switch (data >> 2) {
            case 1: id = blocks::minecraft::chipped_anvil; break;
            case 2: id = blocks::minecraft::damaged_anvil; break;
            case 0:
            default:
                id = blocks::minecraft::anvil;
                break;
            }
            break;
        case 146:
            id = blocks::minecraft::trapped_chest;
            Chest(data, props);
            break;
        case 147: id = blocks::minecraft::light_weighted_pressure_plate; break;
        case 148: id = blocks::minecraft::heavy_weighted_pressure_plate; break;
        case 149:
        case 150:
            id = blocks::minecraft::comparator;
            props["facing"] = FacingN2E3S0W1(data & 0x3);
            props["powered"] = blockId == 150 ? "true" : "false";
            props["mode"] = (data & 0x4) == 0x4 ? "subtract" : "compare";
            break;
        case 151:
            id = blocks::minecraft::daylight_detector;
            props["inverted"] = "false";
            break;
        case 152: id = blocks::minecraft::redstone_block; break;
        case 153: id = blocks::minecraft::nether_quartz_ore; break;
        case 154:
            id = blocks::minecraft::hopper;
            switch (data & 0x7) {
            case 2:
                props["facing"] = "north";
                break;
            case 3:
                props["facing"] = "south";
                break;
            case 4:
                props["facing"] = "west";
                break;
            case 5:
                props["facing"] = "east";
                break;
            case 0:
            default:
                props["facing"] = "down";
                break;
            }
            props["enabled"] = (data & 0x8) == 0x8 ? "false" : "true";
            break;
        case 155:
            switch (data) {
            case 1:
                id = blocks::minecraft::chiseled_quartz_block;
                break;
            case 3:
                id = blocks::minecraft::quartz_pillar;
                props["axis"] = "x";
                break;
            case 4:
                id = blocks::minecraft::quartz_pillar;
                props["axis"] = "z";
                break;
            case 2:
                id = blocks::minecraft::quartz_pillar;
                props["axis"] = "y";
                break;
            case 0:
            default:
                id = blocks::minecraft::quartz_block;
                break;
            }
            break;
        case 156:
            id = blocks::minecraft::quartz_stairs;
            Stairs(data, props);
            break;
        case 157:
            id = blocks::minecraft::activator_rail;
            PoweredRail(data, props);
            break;
        case 158:
            id = blocks::minecraft::dropper;
            props["facing"] = FacingA(data);
            break;
        case 159:
            switch (data) {
            case 1: id = blocks::minecraft::orange_terracotta; break;
            case 2: id = blocks::minecraft::magenta_terracotta; break;
            case 3: id = blocks::minecraft::light_blue_terracotta; break;
            case 4: id = blocks::minecraft::yellow_terracotta; break;
            case 5: id = blocks::minecraft::lime_terracotta; break;
            case 6: id = blocks::minecraft::pink_terracotta; break;
            case 7: id = blocks::minecraft::gray_terracotta; break;
            case 8: id = blocks::minecraft::light_gray_terracotta; break;
            case 9: id = blocks::minecraft::cyan_terracotta; break;
            case 10: id = blocks::minecraft::purple_terracotta; break;
            case 11: id = blocks::minecraft::blue_terracotta; break;
            case 12: id = blocks::minecraft::brown_terracotta; break;
            case 13: id = blocks::minecraft::green_terracotta; break;
            case 14: id = blocks::minecraft::red_terracotta; break;
            case 15: id = blocks::minecraft::black_terracotta; break;
            default:
            case 0:
                id = blocks::minecraft::white_terracotta;
                break;
            }
            break;
        case 160:
            switch (data) {
            case 1: id = blocks::minecraft::orange_stained_glass_pane; break;
            case 2: id = blocks::minecraft::magenta_stained_glass_pane; break;
            case 3: id = blocks::minecraft::light_blue_stained_glass_pane; break;
            case 4: id = blocks::minecraft::yellow_stained_glass_pane; break;
            case 5: id = blocks::minecraft::lime_stained_glass_pane; break;
            case 6: id = blocks::minecraft::pink_stained_glass_pane; break;
            case 7: id = blocks::minecraft::gray_stained_glass_pane; break;
            case 8: id = blocks::minecraft::light_gray_stained_glass_pane; break;
            case 9: id = blocks::minecraft::cyan_stained_glass_pane; break;
            case 10: id = blocks::minecraft::purple_stained_glass_pane; break;
            case 11: id = blocks::minecraft::blue_stained_glass_pane; break;
            case 12: id = blocks::minecraft::brown_stained_glass_pane; break;
            case 13: id = blocks::minecraft::green_stained_glass_pane; break;
            case 14: id = blocks::minecraft::red_stained_glass_pane; break;
            case 15: id = blocks::minecraft::black_stained_glass_pane; break;
            case 0:
            default:
                id = blocks::minecraft::white_stained_glass_pane;
                break;
            }
            break;
        case 161:
            switch (data & 0x3) {
            case 1:
                id = blocks::minecraft::dark_oak_leaves;
                break;
            case 0:
            default:
                id = blocks::minecraft::acacia_leaves;
                break;
            }
            Leaves(data, props);
            break;
        case 162:
            switch (data & 0x3) {
            case 1: id = blocks::minecraft::dark_oak_log; break;
            case 0:
            default:
                id = blocks::minecraft::acacia_log;
                break;
            }
            Log(data, props);
            break;
        case 163:
            id = blocks::minecraft::acacia_stairs;
            Stairs(data, props);
            break;
        case 164:
            id = blocks::minecraft::dark_oak_stairs;
            Stairs(data, props);
            break;
        case 165: id = blocks::minecraft::slime_block; break;
        case 166: id = blocks::minecraft::barrier; break;
        case 167:
            id = blocks::minecraft::iron_trapdoor;
            Trapdoor(data, props);
            break;
        case 168:
            switch (data) {
            case 1: id = blocks::minecraft::prismarine_bricks; break;
            case 2: id = blocks::minecraft::dark_prismarine; break;
            case 0:
            default:
                id = blocks::minecraft::prismarine;
            }
            break;
        case 169: id = blocks::minecraft::sea_lantern; break;
        case 170:
            id = blocks::minecraft::hay_block;
            props["axis"] = Axis(data >> 2);
            break;
        case 171:
            switch (data) {
            case 1: id = blocks::minecraft::orange_carpet; break;
            case 2: id = blocks::minecraft::magenta_carpet; break;
            case 3: id = blocks::minecraft::light_blue_carpet; break;
            case 4: id = blocks::minecraft::yellow_carpet; break;
            case 5: id = blocks::minecraft::lime_carpet; break;
            case 6: id = blocks::minecraft::pink_carpet; break;
            case 7: id = blocks::minecraft::gray_carpet; break;
            case 8: id = blocks::minecraft::light_gray_carpet; break;
            case 9: id = blocks::minecraft::cyan_carpet; break;
            case 10: id = blocks::minecraft::purple_carpet; break;
            case 11: id = blocks::minecraft::blue_carpet; break;
            case 12: id = blocks::minecraft::brown_carpet; break;
            case 13: id = blocks::minecraft::green_carpet; break;
            case 14: id = blocks::minecraft::red_carpet; break;
            case 15: id = blocks::minecraft::black_carpet; break;
            case 0:
            default:
                id = blocks::minecraft::white_carpet;
                break;
            }
            break;
        case 172: id = blocks::minecraft::terracotta; break;
        case 173: id = blocks::minecraft::coal_block; break;
        case 174: id = blocks::minecraft::packed_ice; break;
        case 175:
            switch (data) {
            case 1: id = blocks::minecraft::lilac; break;
            case 2: id = blocks::minecraft::tall_grass; break;
            case 3: id = blocks::minecraft::large_fern; break;
            case 4: id = blocks::minecraft::rose_bush; break;
            case 5: id = blocks::minecraft::peony; break;
            case 0:
            default:
                id = blocks::minecraft::sunflower;
                break;
            }
            break;
        case 176:
            id = blocks::minecraft::white_banner;
            props["rotation"] = std::to_string(data);
            break;
        case 177:
            id = blocks::minecraft::white_wall_banner;
            switch (data) {
            case 3:
                props["facing"] = "south";
                break;
            case 4:
                props["facing"] = "west";
                break;
            case 5:
                props["facing"] = "east";
                break;
            case 2:
            default:
                props["facing"] = "north";
                break;
            }
            break;
        case 178:
            id = blocks::minecraft::daylight_detector;
            props["inverted"] = "true";
            break;
        case 179:
            id = blocks::minecraft::red_sandstone;
            switch (data) {
            case 1: id = blocks::minecraft::chiseled_red_sandstone; break;
            case 2: id = blocks::minecraft::cut_red_sandstone; break;
            case 0:
            default:
                id = blocks::minecraft::red_sandstone;
                break;
            }
            break;
        case 180:
            id = blocks::minecraft::red_sandstone_stairs;
            Stairs(data, props);
            break;
        case 181:
            id = blocks::minecraft::red_sandstone_slab;
            props["type"] = "double";
            break;
        case 182:
            switch (data) {
            case 8:
                id = blocks::minecraft::red_sandstone_slab;
                props["type"] = "top";
                break;
            case 0:
            default:
                id = blocks::minecraft::red_sandstone_slab;
                props["type"] = "bottom";
                break;
            }
            break;
        case 183: id = blocks::minecraft::spruce_fence_gate; break;
        case 184: id = blocks::minecraft::birch_fence_gate; break;
        case 185: id = blocks::minecraft::jungle_fence_gate; break;
        case 186: id = blocks::minecraft::dark_oak_fence_gate; break;
        case 187: id = blocks::minecraft::acacia_fence_gate; break;
        case 188: id = blocks::minecraft::spruce_fence; break;
        case 189: id = blocks::minecraft::birch_fence; break;
        case 190: id = blocks::minecraft::jungle_fence; break;
        case 191: id = blocks::minecraft::dark_oak_fence; break;
        case 192: id = blocks::minecraft::acacia_fence; break;
        case 193:
            id = blocks::minecraft::spruce_door;
            Door(data, props);
            break;
        case 194:
            id = blocks::minecraft::birch_door;
            Door(data, props);
            break;
        case 195:
            id = blocks::minecraft::jungle_door;
            Door(data, props);
            break;
        case 196:
            id = blocks::minecraft::acacia_door;
            Door(data, props);
            break;
        case 197:
            id = blocks::minecraft::dark_oak_door;
            Door(data, props);
            break;
        case 198:
            id = blocks::minecraft::end_rod;
            props["facing"] = FacingA(data);
            break;
        case 199: id = blocks::minecraft::chorus_plant; break;
        case 200:
            id = blocks::minecraft::chorus_flower;
            props["age"] = std::to_string(data);
            break;
        case 201: id = blocks::minecraft::purpur_block; break;
        case 202:
            id = blocks::minecraft::purpur_pillar;
            props["axis"] = Axis(data >> 2);
            break;
        case 203:
            id = blocks::minecraft::purpur_stairs;
            Stairs(data, props);
            break;
        case 204:
            id = blocks::minecraft::purpur_slab;
            props["type"] = "double";
            break;
        case 205:
            id = blocks::minecraft::purpur_slab;
            if (data == 8) {
                props["type"] = "top";
            } else {
                props["type"] = "bottom";
            }
            break;
        case 206: id = blocks::minecraft::end_stone_bricks; break;
        case 207:
            id = blocks::minecraft::beetroots;
            props["age"] = std::to_string(data);
            break;
        case 208: id = blocks::minecraft::dirt_path; break;
        case 209: id = blocks::minecraft::end_gateway; break;
        case 210: id = blocks::minecraft::repeating_command_block; break;
        case 211: id = blocks::minecraft::chain_command_block; break;
        case 212: id = blocks::minecraft::frosted_ice; break;
        case 213: id = blocks::minecraft::magma_block; break;
        case 214: id = blocks::minecraft::nether_wart_block; break;
        case 215: id = blocks::minecraft::red_nether_bricks; break;
        case 216:
            id = blocks::minecraft::bone_block;
            props["axis"] = Axis(data >> 2);
            break;
        case 217: id = blocks::minecraft::structure_void; break;
        case 218:
            id = blocks::minecraft::observer;
            props["facing"] = FacingA(data);
            break;
        case 219:
            id = blocks::minecraft::white_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 220:
            id = blocks::minecraft::orange_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 221:
            id = blocks::minecraft::magenta_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 222:
            id = blocks::minecraft::light_blue_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 223:
            id = blocks::minecraft::yellow_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 224:
            id = blocks::minecraft::lime_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 225:
            id = blocks::minecraft::pink_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 226:
            id = blocks::minecraft::gray_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 227:
            id = blocks::minecraft::light_gray_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 228:
            id = blocks::minecraft::cyan_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 229:
            id = blocks::minecraft::shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 230:
            id = blocks::minecraft::blue_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 231:
            id = blocks::minecraft::brown_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 232:
            id = blocks::minecraft::green_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 233:
            id = blocks::minecraft::red_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 234:
            id = blocks::minecraft::black_shulker_box;
            props["facing"] = FacingA(data);
            break;
        case 235:
            id = blocks::minecraft::white_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 236:
            id = blocks::minecraft::orange_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 237:
            id = blocks::minecraft::magenta_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 238:
            id = blocks::minecraft::light_blue_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 239:
            id = blocks::minecraft::yellow_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 240:
            id = blocks::minecraft::lime_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 241:
            id = blocks::minecraft::pink_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 242:
            id = blocks::minecraft::gray_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 243:
            id = blocks::minecraft::light_gray_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 244:
            id = blocks::minecraft::cyan_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 245:
            id = blocks::minecraft::purple_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 246:
            id = blocks::minecraft::blue_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 247:
            id = blocks::minecraft::brown_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 248:
            id = blocks::minecraft::green_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 249:
            id = blocks::minecraft::red_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 250:
            id = blocks::minecraft::black_glazed_terracotta;
            GlazedTerracotta(data, props);
            break;
        case 251:
            switch (data) {
            case 1: id = blocks::minecraft::orange_concrete; break;
            case 2: id = blocks::minecraft::magenta_concrete; break;
            case 3: id = blocks::minecraft::light_blue_concrete; break;
            case 4: id = blocks::minecraft::yellow_concrete; break;
            case 5: id = blocks::minecraft::lime_concrete; break;
            case 6: id = blocks::minecraft::pink_concrete; break;
            case 7: id = blocks::minecraft::gray_concrete; break;
            case 8: id = blocks::minecraft::light_gray_concrete; break;
            case 9: id = blocks::minecraft::cyan_concrete; break;
            case 10: id = blocks::minecraft::purple_concrete; break;
            case 11: id = blocks::minecraft::blue_concrete; break;
            case 12: id = blocks::minecraft::brown_concrete; break;
            case 13: id = blocks::minecraft::green_concrete; break;
            case 14: id = blocks::minecraft::red_concrete; break;
            case 15: id = blocks::minecraft::black_concrete; break;
            case 0:
            default:
                id = blocks::minecraft::white_concrete;
                break;
            }
            break;
        case 252:
            switch (data) {
            case 1: id = blocks::minecraft::orange_concrete_powder; break;
            case 2: id = blocks::minecraft::magenta_concrete_powder; break;
            case 3: id = blocks::minecraft::light_blue_concrete_powder; break;
            case 4: id = blocks::minecraft::yellow_concrete_powder; break;
            case 5: id = blocks::minecraft::lime_concrete_powder; break;
            case 6: id = blocks::minecraft::pink_concrete_powder; break;
            case 7: id = blocks::minecraft::gray_concrete_powder; break;
            case 8: id = blocks::minecraft::light_gray_concrete_powder; break;
            case 9: id = blocks::minecraft::cyan_concrete_powder; break;
            case 10: id = blocks::minecraft::purple_concrete_powder; break;
            case 11: id = blocks::minecraft::blue_concrete_powder; break;
            case 12: id = blocks::minecraft::brown_concrete_powder; break;
            case 13: id = blocks::minecraft::green_concrete_powder; break;
            case 14: id = blocks::minecraft::red_concrete_powder; break;
            case 15: id = blocks::minecraft::black_concrete_powder; break;
            case 0:
            default:
                id = blocks::minecraft::white_concrete_powder;
                break;
            }
            break;
        case 255: id = blocks::minecraft::structure_block; break;
        default: id = blocks::minecraft::air; break;
        }
        return std::make_shared<mcfile::je::Block>(id, props);
    }

    static inline uint8_t Nibble4(std::vector<uint8_t> const &arr, int index) {
        return index % 2 == 0 ? arr[index / 2] & 0x0F : (arr[index / 2] >> 4) & 0x0F;
    }

    static void Stairs(uint8_t data, std::map<std::string, std::string> &props) {
        static std::string const facing[4] = {"east", "west", "south", "north"};
        props["facing"] = facing[std::clamp(data & 0x3, 0, 3)];
        static std::string const half[2] = {"bottm", "top"};
        props["half"] = half[std::clamp(data & 0x4, 0, 1)];
    }

    static void Door(uint8_t data, std::map<std::string, std::string> &props) {
        static std::string const facing[4] = {"east", "south", "west", "north"};
        static std::string const hinge[2] = {"left", "right"};
        static std::string const open[2] = {"false", "true"};
        static std::string const powered[2] = {"false", "true"};
        if (data < 8) {
            props["half"] = "lower";
            props["facing"] = facing[std::clamp(data & 0x3, 0, 3)];
            props["open"] = open[std::clamp((data >> 2) & 0x1, 0, 1)];
        } else {
            props["half"] = "upper";
            props["hinge"] = hinge[std::clamp(data & 0x1, 0, 1)];
            props["powered"] = powered[std::clamp((data >> 1) & 0x1, 0, 1)];
        }
    }

    static std::string Axis(uint8_t v) {
        static std::string const axis[3] = {"y", "x", "z"};
        return axis[std::clamp<uint8_t>(v, 0, 2)];
    }

    static void Log(uint8_t data, std::map<std::string, std::string> &props) {
        props["axis"] = Axis((data >> 2) & 0x3);
    }

    static void Leaves(uint8_t data, std::map<std::string, std::string> &props) {
        if (((data >> 2) & 0x1) == 0x1) {
            props["persistent"] = "true";
        } else {
            props["persistent"] = "false";
        }
    }

    static void Bed(uint8_t data, std::map<std::string, std::string> &props) {
        switch (data & 0x3) {
        case 2:
            props["facing"] = "north";
            break;
        case 3:
            props["facing"] = "east";
            break;
        case 1:
            props["facing"] = "west";
            break;
        case 0:
        default:
            props["facing"] = "south";
            break;
        }
        props["occupied"] = ((data >> 2) & 0x1) == 0x1 ? "true" : "false";
        props["part"] = ((data >> 3) & 0x1) == 0x1 ? "head" : "foot";
    }

    static void Rail(uint8_t data, std::map<std::string, std::string> &props) {
        static std::string const shape[10] = {
            "north_south",     // 0
            "east_west",       // 1
            "ascending_east",  // 2
            "ascending_west",  // 3
            "ascending_north", // 4
            "ascending_south", // 5
            "south_east",      // 6
            "south_west",      // 7
            "north_west",      // 8
            "north_east",      // 9
        };
        props["shape"] = shape[std::clamp<uint8_t>(data, 0, 9)];
    }

    static void PoweredRail(uint8_t data, std::map<std::string, std::string> &props) {
        static std::string const shape[6] = {
            "north_south",     // 0
            "east_west",       // 1
            "ascending_east",  // 2
            "ascending_west",  // 3
            "ascending_north", // 4
            "ascending_south"  // 5
        };
        props["shape"] = shape[std::clamp(data & 0x7, 0, 5)];
        props["powered"] = ((data >> 3) & 0x1) == 0x1 ? "true" : "false";
    }

    static std::string FacingA(uint8_t data) {
        static std::string const facing[6] = {"down", "up", "north", "south", "west", "east"};
        return facing[std::clamp<uint8_t>(data, 0, 5)];
    }

    static void Piston(uint8_t data, std::map<std::string, std::string> &props) {
        auto facing = FacingA(data & 0x7);
        props["facing"] = facing;
        props["extended"] = ((data >> 3) & 0x1) == 0x1 ? "true" : "false";
    }

    static std::string FacingB(uint8_t data) {
        static std::string const facing[4] = {"south", "west", "north", "east"};
        return facing[std::clamp<uint8_t>(data, 0, 3)];
    }

    static void Chest(uint8_t data, std::map<std::string, std::string> &props) {
        switch (data) {
        case 3: props["facing"] = "south"; break;
        case 4: props["facing"] = "west"; break;
        case 5: props["facing"] = "east"; break;
        case 2:
        default:
            props["facing"] = "north";
            break;
        }
    }

    static void GlazedTerracotta(uint8_t data, std::map<std::string, std::string> &props) {
        switch (data) {
        case 0: props["facing"] = "south"; break;
        case 1: props["facing"] = "west"; break;
        case 2: props["facing"] = "north"; break;
        case 3: props["facing"] = "east"; break;
        }
    }

    static void Trapdoor(uint8_t data, std::map<std::string, std::string> &props) {
        switch (data & 0x3) {
        case 1: props["facing"] = "south"; break;
        case 2: props["facing"] = "west"; break;
        case 3: props["facing"] = "east"; break;
        default:
        case 0:
            props["facing"] = "north";
            break;
        }
        props["half"] = (data & 0x8) == 0x8 ? "top" : "bottom";
        props["open"] = (data & 0x4) == 0x4 ? "true" : "fasle";
    }

    static void Button(uint8_t data, std::map<std::string, std::string> &props) {
        switch (data & 0x7) {
        case 0:
            props["face"] = "ceiling";
            break;
        case 1:
            props["face"] = "wall";
            props["facing"] = "east";
            break;
        case 2:
            props["face"] = "wall";
            props["facing"] = "west";
            break;
        case 3:
            props["face"] = "wall";
            props["facing"] = "south";
            break;
        case 4:
            props["face"] = "wall";
            props["facing"] = "north";
            break;
        case 5:
            props["face"] = "floor";
            break;
        }
        props["powered"] = (data & 0x8) == 0x8 ? "true" : "false";
    }

    static void Lever(uint8_t data, std::map<std::string, std::string> &props) {
        switch (data & 0x7) {
        case 0:
            props["face"] = "ceiling";
            props["facing"] = "west";
            break;
        case 1:
            props["face"] = "wall";
            props["facing"] = "east";
            break;
        case 2:
            props["face"] = "wall";
            props["facing"] = "west";
            break;
        case 3:
            props["face"] = "wall";
            props["facing"] = "south";
            break;
        case 4:
            props["face"] = "wall";
            props["facing"] = "north";
            break;
        case 5:
            props["face"] = "floor";
            props["facing"] = "north";
            break;
        case 6:
            props["face"] = "floor";
            props["facing"] = "west";
            break;
        case 7:
            props["face"] = "ceiling";
            props["facing"] = "north";
            break;
        }
        props["powered"] = (data & 0x8) == 0x8 ? "true" : "false";
    }

    static std::string FacingN2E3S0W1(uint8_t data) {
        switch (data) {
        case 1: return "west";
        case 2: return "north";
        case 3: return "east";
        case 0:
        default:
            return "south";
        }
    }

    static mcfile::blocks::BlockId MushroomBlock(mcfile::blocks::BlockId id, uint8_t data, std::map<std::string, std::string> &props) {
        switch (data) {
        case 1:
            props["down"] = "false";
            props["east"] = "false";
            props["north"] = "true";
            props["south"] = "false";
            props["up"] = "true";
            props["west"] = "true";
            return id;
        case 2:
            props["down"] = "false";
            props["east"] = "false";
            props["north"] = "true";
            props["south"] = "false";
            props["up"] = "true";
            props["west"] = "false";
            return id;
        case 3:
            props["down"] = "false";
            props["east"] = "true";
            props["north"] = "true";
            props["south"] = "false";
            props["up"] = "true";
            props["west"] = "false";
            return id;
        case 4:
            props["down"] = "false";
            props["east"] = "false";
            props["north"] = "false";
            props["south"] = "false";
            props["up"] = "true";
            props["west"] = "true";
            return id;
        case 5:
            props["down"] = "false";
            props["east"] = "false";
            props["north"] = "false";
            props["south"] = "false";
            props["up"] = "true";
            props["west"] = "false";
            return id;
        case 6:
            props["down"] = "false";
            props["east"] = "true";
            props["north"] = "false";
            props["south"] = "false";
            props["up"] = "true";
            props["west"] = "false";
            return id;
        case 7:
            props["down"] = "false";
            props["east"] = "false";
            props["north"] = "false";
            props["south"] = "true";
            props["up"] = "true";
            props["west"] = "true";
            return id;
        case 8:
            props["down"] = "false";
            props["east"] = "false";
            props["north"] = "false";
            props["south"] = "true";
            props["up"] = "true";
            props["west"] = "false";
            return id;
        case 9:
            props["down"] = "false";
            props["east"] = "true";
            props["north"] = "false";
            props["south"] = "true";
            props["up"] = "true";
            props["west"] = "false";
            return id;
        case 10:
            props["down"] = "false";
            props["up"] = "false";
            props["east"] = "true";
            props["south"] = "true";
            props["west"] = "true";
            props["north"] = "true";
            return blocks::minecraft::mushroom_stem;
        case 11:
            props["down"] = "true";
            props["up"] = "true";
            props["east"] = "true";
            props["south"] = "true";
            props["west"] = "true";
            props["north"] = "true";
            return id;
        case 0:
        default:
            props["down"] = "false";
            props["up"] = "false";
            props["east"] = "false";
            props["south"] = "false";
            props["west"] = "false";
            props["north"] = "false";
            return id;
        }
    }

private:
    static int16_t DrainDamage(int16_t *damage) {
        int16_t v = *damage;
        *damage = 0;
        return v;
    }
};
} // namespace mcfile::je
