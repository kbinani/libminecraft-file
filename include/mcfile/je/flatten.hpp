#pragma once

namespace mcfile::je {
class Flatten {
    Flatten() = delete;

public:
    static std::optional<std::u8string> Item(std::u8string const &id, int16_t *damage) {
        using namespace std;
        int16_t safety = 0;
        assert(damage);
        if (!damage) {
            damage = &safety;
        }
        std::u8string_view i(id);
        if (i.starts_with(u8"minecraft:")) {
            i = i.substr(10);
        }

#if defined(ns)
#error "ns" macro already defined
#endif
#define ns u8"minecraft:"

        if (i == u8"stone") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"granite";
            case 2: return ns u8"polished_granite";
            case 3: return ns u8"diorite";
            case 4: return ns u8"polished_diorite";
            case 5: return ns u8"andesite";
            case 6: return ns u8"polished_andesite";
            case 0:
            default:
                return ns u8"stone";
            }
        }
        if (i == u8"grass") {
            return ns u8"grass_block";
        }
        if (i == u8"dirt") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"coarse_dirt";
            case 2: return ns u8"podzol";
            case 0:
            default:
                return ns u8"dirt";
            }
        }
        if (i == u8"planks") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"spruce_planks";
            case 2: return ns u8"birch_planks";
            case 3: return ns u8"jungle_planks";
            case 4: return ns u8"acacia_planks";
            case 5: return ns u8"dark_oak_planks";
            case 0:
            default:
                return ns u8"oak_planks";
            }
        }
        if (i == u8"sand") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"red_sand";
            case 0:
            default:
                return ns u8"sand";
            }
        }
        if (i == u8"log") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"spruce_log";
            case 2: return ns u8"birch_log";
            case 3: return ns u8"jungle_log";
            case 0:
            default:
                return ns u8"oak_log";
            }
        }
        if (i == u8"sponge") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"wet_sponge";
            case 0:
            default:
                return ns u8"sponge";
            }
        }
        if (i == u8"sandstone") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"chiseled_sandstone";
            case 2: return ns u8"cut_sandstone";
            case 0:
            default:
                return ns u8"sandstone";
            }
        }
        if (i == u8"wool") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_wool";
            case 2: return ns u8"magenta_wool";
            case 3: return ns u8"light_blue_wool";
            case 4: return ns u8"yellow_wool";
            case 5: return ns u8"lime_wool";
            case 6: return ns u8"pink_wool";
            case 7: return ns u8"gray_wool";
            case 8: return ns u8"light_gray_wool";
            case 9: return ns u8"cyan_wool";
            case 10: return ns u8"purple_wool";
            case 11: return ns u8"blue_wool";
            case 12: return ns u8"brown_wool";
            case 13: return ns u8"green_wool";
            case 14: return ns u8"red_wool";
            case 15: return ns u8"black_wool";
            case 0:
            default:
                return ns u8"white_wool";
            }
        }
        if (i == u8"stone_slab") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"sandstone_slab";
            case 3: return ns u8"cobblestone_slab";
            case 4: return ns u8"brick_slab";
            case 5: return ns u8"stone_brick_slab";
            case 6: return ns u8"nether_brick_slab";
            case 7: return ns u8"quartz_slab";
            case 0:
            default:
                return ns u8"smooth_stone_slab";
            }
        }
        if (i == u8"brick_block") {
            return ns u8"bricks";
        }
        if (i == u8"stone_stairs") {
            return ns u8"cobblestone_stairs";
        }
        if (i == u8"snow") {
            return ns u8"snow_block";
        }
        if (i == u8"pumpkin") {
            return ns u8"carved_pumpkin";
        }
        if (i == u8"lit_pumpkin") {
            return ns u8"jack_o_lantern";
        }
        if (i == u8"stained_glass") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_stained_glass";
            case 2: return ns u8"magenta_stained_glass";
            case 3: return ns u8"light_blue_stained_glass";
            case 4: return ns u8"yellow_stained_glass";
            case 5: return ns u8"lime_stained_glass";
            case 6: return ns u8"pink_stained_glass";
            case 7: return ns u8"gray_stained_glass";
            case 8: return ns u8"light_gray_stained_glass";
            case 9: return ns u8"cyan_stained_glass";
            case 10: return ns u8"purple_stained_glass";
            case 11: return ns u8"blue_stained_glass";
            case 12: return ns u8"brown_stained_glass";
            case 13: return ns u8"green_stained_glass";
            case 14: return ns u8"red_stained_glass";
            case 15: return ns u8"black_stained_glass";
            case 0:
            default:
                return ns u8"white_stained_glass";
            }
        }
        if (i == u8"stonebrick") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"mossy_stone_bricks";
            case 2: return ns u8"cracked_stone_bricks";
            case 3: return ns u8"chiseled_stone_bricks";
            case 0:
            default:
                return ns u8"stone_bricks";
            }
        }
        if (i == u8"melon_block") {
            return ns u8"melon";
        }
        if (i == u8"nether_brick") {
            return ns u8"nether_bricks";
        }
        if (i == u8"wooden_slab") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"spruce_slab";
            case 2: return ns u8"birch_slab";
            case 3: return ns u8"jungle_slab";
            case 4: return ns u8"acacia_slab";
            case 5: return ns u8"dark_oak_slab";
            case 0:
            default:
                return ns u8"oak_slab";
            }
        }
        if (i == u8"quartz_block") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"chiseled_quartz_block";
            case 2: return ns u8"quartz_pillar";
            case 0:
            default:
                return ns u8"quartz_block";
            }
        }
        if (i == u8"stained_hardened_clay") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_terracotta";
            case 2: return ns u8"magenta_terracotta";
            case 3: return ns u8"light_blue_terracotta";
            case 4: return ns u8"yellow_terracotta";
            case 5: return ns u8"lime_terracotta";
            case 6: return ns u8"pink_terracotta";
            case 7: return ns u8"gray_terracotta";
            case 8: return ns u8"light_gray_terracotta";
            case 9: return ns u8"cyan_terracotta";
            case 10: return ns u8"purple_terracotta";
            case 11: return ns u8"blue_terracotta";
            case 12: return ns u8"brown_terracotta";
            case 13: return ns u8"green_terracotta";
            case 14: return ns u8"red_terracotta";
            case 15: return ns u8"black_terracotta";
            case 0:
            default:
                return ns u8"white_terracotta";
            }
        }
        if (i == u8"log2") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"dark_oak_log";
            case 0:
            default:
                return ns u8"acacia_log";
            }
        }
        if (i == u8"prismarine") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"prismarine_bricks";
            case 2: return ns u8"dark_prismarine";
            case 0:
            default:
                return ns u8"prismarine";
            }
        }
        if (i == u8"hardened_clay") {
            return ns u8"terracotta";
        }
        if (i == u8"red_sandstone") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"chiseled_red_sandstone";
            case 2: return ns u8"cut_red_sandstone";
            case 0:
            default:
                return ns u8"red_sandstone";
            }
        }
        if (i == u8"stone_slab2") {
            return ns u8"red_sandstone_slab";
        }
        if (i == u8"magma") {
            return ns u8"magma_block";
        }
        if (i == u8"red_nether_brick") {
            return ns u8"red_nether_bricks";
        }
        if (i == u8"concrete") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_concrete";
            case 2: return ns u8"magenta_concrete";
            case 3: return ns u8"light_blue_concrete";
            case 4: return ns u8"yellow_concrete";
            case 5: return ns u8"lime_concrete";
            case 6: return ns u8"pink_concrete";
            case 7: return ns u8"gray_concrete";
            case 8: return ns u8"light_gray_concrete";
            case 9: return ns u8"cyan_concrete";
            case 10: return ns u8"purple_concrete";
            case 11: return ns u8"blue_concrete";
            case 12: return ns u8"brown_concrete";
            case 13: return ns u8"green_concrete";
            case 14: return ns u8"red_concrete";
            case 15: return ns u8"black_concrete";
            case 0:
            default:
                return ns u8"white_concrete";
            }
        }
        if (i == u8"concrete_powder") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_concrete_powder";
            case 2: return ns u8"magenta_concrete_powder";
            case 3: return ns u8"light_blue_concrete_powder";
            case 4: return ns u8"yellow_concrete_powder";
            case 5: return ns u8"lime_concrete_powder";
            case 6: return ns u8"pink_concrete_powder";
            case 7: return ns u8"gray_concrete_powder";
            case 8: return ns u8"light_gray_concrete_powder";
            case 9: return ns u8"cyan_concrete_powder";
            case 10: return ns u8"purple_concrete_powder";
            case 11: return ns u8"blue_concrete_powder";
            case 12: return ns u8"brown_concrete_powder";
            case 13: return ns u8"green_concrete_powder";
            case 14: return ns u8"red_concrete_powder";
            case 15: return ns u8"black_concrete_powder";
            case 0:
            default:
                return ns u8"white_concrete_powder";
            }
        }
        if (i == u8"sapling") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"spruce_sapling";
            case 2: return ns u8"birch_sapling";
            case 3: return ns u8"jungle_sapling";
            case 4: return ns u8"acacia_sapling";
            case 5: return ns u8"dark_oak_sapling";
            case 0:
            default:
                return ns u8"oak_sapling";
            }
        }
        if (i == u8"leaves") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"spruce_leaves";
            case 2: return ns u8"birch_leaves";
            case 3: return ns u8"jungle_leaves";
            case 0:
            default:
                return ns u8"oak_leaves";
            }
        }
        if (i == u8"web") {
            return ns u8"cobweb";
        }
        if (i == u8"tallgrass") {
            switch (DrainDamage(damage)) {
            case 2: return ns u8"fern";
            case 1:
            default:
                return ns u8"grass";
            }
        }
        if (i == u8"deadbush") {
            return ns u8"dead_bush";
        }
        if (i == u8"yellow_flower") {
            return ns u8"dandelion";
        }
        if (i == u8"red_flower") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"blue_orchid";
            case 2: return ns u8"allium";
            case 3: return ns u8"azure_bluet";
            case 4: return ns u8"red_tulip";
            case 5: return ns u8"orange_tulip";
            case 6: return ns u8"white_tulip";
            case 7: return ns u8"pink_tulip";
            case 8: return ns u8"oxeye_daisy";
            case 0:
            default:
                return ns u8"poppy";
            }
        }
        if (i == u8"monster_egg") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"infested_cobblestone";
            case 2: return ns u8"infested_stone_bricks";
            case 3: return ns u8"infested_mossy_stone_bricks";
            case 4: return ns u8"infested_cracked_stone_bricks";
            case 5: return ns u8"infested_chiseled_stone_bricks";
            case 0:
            default:
                return ns u8"infested_stone";
            }
        }
        if (i == u8"cobblestone_wall") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"mossy_cobblestone_wall";
            case 0:
            default:
                return ns u8"cobblestone_wall";
            }
        }
        if (i == u8"anvil") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"chipped_anvil";
            case 2: return ns u8"damaged_anvil";
            case 0:
            default:
                return ns u8"anvil";
            }
        }
        if (i == u8"stained_glass_pane") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_stained_glass_pane";
            case 2: return ns u8"magenta_stained_glass_pane";
            case 3: return ns u8"light_blue_stained_glass_pane";
            case 4: return ns u8"yellow_stained_glass_pane";
            case 5: return ns u8"lime_stained_glass_pane";
            case 6: return ns u8"pink_stained_glass_pane";
            case 7: return ns u8"gray_stained_glass_pane";
            case 8: return ns u8"light_gray_stained_glass_pane";
            case 9: return ns u8"cyan_stained_glass_pane";
            case 10: return ns u8"purple_stained_glass_pane";
            case 11: return ns u8"blue_stained_glass_pane";
            case 12: return ns u8"brown_stained_glass_pane";
            case 13: return ns u8"green_stained_glass_pane";
            case 14: return ns u8"red_stained_glass_pane";
            case 15: return ns u8"black_stained_glass_pane";
            case 0:
            default:
                return ns u8"white_stained_glass_pane";
            }
        }
        if (i == u8"leaves2") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"dark_oak_leaves";
            case 0:
            default:
                return ns u8"acacia_leaves";
            }
        }
        if (i == u8"slime") {
            return ns u8"slime_block";
        }
        if (i == u8"carpet") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_carpet";
            case 2: return ns u8"magenta_carpet";
            case 3: return ns u8"light_blue_carpet";
            case 4: return ns u8"yellow_carpet";
            case 5: return ns u8"lime_carpet";
            case 6: return ns u8"pink_carpet";
            case 7: return ns u8"gray_carpet";
            case 8: return ns u8"light_gray_carpet";
            case 9: return ns u8"cyan_carpet";
            case 10: return ns u8"purple_carpet";
            case 11: return ns u8"blue_carpet";
            case 12: return ns u8"brown_carpet";
            case 13: return ns u8"green_carpet";
            case 14: return ns u8"red_carpet";
            case 15: return ns u8"black_carpet";
            case 0:
            default:
                return ns u8"white_carpet";
            }
        }
        if (i == u8"double_plant") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"lilac";
            case 2: return ns u8"tall_grass";
            case 3: return ns u8"large_fern";
            case 4: return ns u8"rose_bush";
            case 5: return ns u8"peony";
            case 0:
            default:
                return ns u8"sunflower";
            }
        }
        if (i == u8"silver_shulker_box") {
            return ns u8"light_gray_shulker_box";
        }
        if (i == u8"silver_glazed_terracotta") {
            return ns u8"light_gray_glazed_terracotta";
        }
        if (i == u8"bed") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_bed";
            case 2: return ns u8"magenta_bed";
            case 3: return ns u8"light_blue_bed";
            case 4: return ns u8"yellow_bed";
            case 5: return ns u8"lime_bed";
            case 6: return ns u8"pink_bed";
            case 7: return ns u8"gray_bed";
            case 8: return ns u8"light_gray_bed";
            case 9: return ns u8"cyan_bed";
            case 10: return ns u8"purple_bed";
            case 11: return ns u8"blue_bed";
            case 12: return ns u8"brown_bed";
            case 13: return ns u8"green_bed";
            case 14: return ns u8"red_bed";
            case 15: return ns u8"black_bed";
            case 0:
            default:
                return ns u8"white_bed";
            }
        }
        if (i == u8"skull") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"wither_skeleton_skull";
            case 2: return ns u8"zombie_head";
            case 3: return ns u8"player_head";
            case 4: return ns u8"creeper_head";
            case 5: return ns u8"dragon_head";
            case 0:
            default:
                return ns u8"skeleton_skull";
            }
        }
        if (i == u8"banner") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"orange_banner";
            case 2: return ns u8"magenta_banner";
            case 3: return ns u8"light_blue_banner";
            case 4: return ns u8"yellow_banner";
            case 5: return ns u8"lime_banner";
            case 6: return ns u8"pink_banner";
            case 7: return ns u8"gray_banner";
            case 8: return ns u8"light_gray_banner";
            case 9: return ns u8"cyan_banner";
            case 10: return ns u8"purple_banner";
            case 11: return ns u8"blue_banner";
            case 12: return ns u8"brown_banner";
            case 13: return ns u8"green_banner";
            case 14: return ns u8"red_banner";
            case 15: return ns u8"black_banner";
            case 0:
            default:
                return ns u8"white_banner";
            }
        }
        if (i == u8"noteblock") {
            return ns u8"note_block";
        }
        if (i == u8"wooden_pressure_plate") {
            return ns u8"oak_pressure_plate";
        }
        if (i == u8"trapdoor") {
            return ns u8"oak_trapdoor";
        }
        if (i == u8"fence_gate") {
            return ns u8"oak_fence_gate";
        }
        if (i == u8"wooden_button") {
            return ns u8"oak_button";
        }
        if (i == u8"wooden_door") {
            return ns u8"oak_door";
        }
        if (i == u8"golden_rail") {
            return ns u8"powered_rail";
        }
        if (i == u8"boat") {
            return ns u8"oak_boat";
        }
        if (i == u8"coal") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"charcoal";
            case 0:
            default:
                return ns u8"coal";
            }
        }
        if (i == u8"dye") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"red_dye";
            case 2: return ns u8"green_dye";
            case 3: return ns u8"cocoa_beans";
            case 4: return ns u8"lapis_lazuli";
            case 5: return ns u8"purple_dye";
            case 6: return ns u8"cyan_dye";
            case 7: return ns u8"light_gray_dye";
            case 8: return ns u8"gray_dye";
            case 9: return ns u8"pink_dye";
            case 10: return ns u8"lime_dye";
            case 11: return ns u8"yellow_dye";
            case 12: return ns u8"light_blue_dye";
            case 13: return ns u8"magenta_dye";
            case 14: return ns u8"orange_dye";
            case 15: return ns u8"bone_meal";
            case 0:
            default:
                return ns u8"ink_sac";
            }
        }
        if (i == u8"firework_charge") {
            return ns u8"firework_star";
        }
        if (i == u8"netherbrick") {
            return ns u8"nether_brick";
        }
        if (i == u8"chorus_fruit_popped") {
            return ns u8"popped_chorus_fruit";
        }
        if (i == u8"record_13") {
            return ns u8"music_disc_13";
        }
        if (i == u8"record_cat") {
            return ns u8"music_disc_cat";
        }
        if (i == u8"record_blocks") {
            return ns u8"music_disc_blocks";
        }
        if (i == u8"record_chirp") {
            return ns u8"music_disc_chirp";
        }
        if (i == u8"record_far") {
            return ns u8"music_disc_far";
        }
        if (i == u8"record_mall") {
            return ns u8"music_disc_mall";
        }
        if (i == u8"record_mellohi") {
            return ns u8"music_disc_mellohi";
        }
        if (i == u8"record_stal") {
            return ns u8"music_disc_stal";
        }
        if (i == u8"record_strad") {
            return ns u8"music_disc_strad";
        }
        if (i == u8"record_ward") {
            return ns u8"music_disc_ward";
        }
        if (i == u8"record_11") {
            return ns u8"music_disc_11";
        }
        if (i == u8"record_wait") {
            return ns u8"music_disc_wait";
        }
        if (i == u8"golden_apple") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"enchanted_golden_apple";
            case 0:
            default:
                return ns u8"golden_apple";
            }
        }
        if (i == u8"fish") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"salmon";
            case 2: return ns u8"tropical_fish";
            case 3: return ns u8"pufferfish";
            case 0:
            default:
                return ns u8"cod";
            }
        } else if (i == u8"cooked_fish") {
            switch (DrainDamage(damage)) {
            case 1: return ns u8"cooked_salmon";
            case 0:
            default:
                return ns u8"cooked_cod";
            }
        }
        if (i == u8"melon") {
            return ns u8"melon_slice";
        }
#undef ns

        return u8"minecraft:" + u8string(i);
    }

    static std::optional<std::u8string> Item(int16_t id, int16_t *damage) {
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
                return u8string(block->fName);
            }
        }
#define ns u8"minecraft:"
#define egg u8"_spawn_egg";
        switch (id) {
        case 256: return ns u8"iron_shovel";
        case 257: return ns u8"iron_pickaxe";
        case 258: return ns u8"iron_axe";
        case 259: return ns u8"flint_and_steel";
        case 260: return ns u8"apple";
        case 261: return ns u8"bow";
        case 262: return ns u8"arrow";
        case 263:
            switch (DrainDamage(damage)) {
            case 1: return ns u8"charcoal";
            case 0:
            default:
                return ns u8"coal";
            }
        case 264: return ns u8"diamond";
        case 265: return ns u8"iron_ingot";
        case 266: return ns u8"gold_ingot";
        case 267: return ns u8"iron_sword";
        case 268: return ns u8"wooden_sword";
        case 269: return ns u8"wooden_shovel";
        case 270: return ns u8"wooden_pickaxe";
        case 271: return ns u8"wooden_axe";
        case 272: return ns u8"stone_sword";
        case 273: return ns u8"stone_shovel";
        case 274: return ns u8"stone_pickaxe";
        case 275: return ns u8"stone_axe";
        case 276: return ns u8"diamond_sword";
        case 277: return ns u8"diamond_shovel";
        case 278: return ns u8"diamond_pickaxe";
        case 279: return ns u8"diamond_axe";
        case 280: return ns u8"stick";
        case 281: return ns u8"bowl";
        case 282: return ns u8"mushroom_stew";
        case 283: return ns u8"golden_sword";
        case 284: return ns u8"golden_shovel";
        case 285: return ns u8"golden_pickaxe";
        case 286: return ns u8"golden_axe";
        case 287: return ns u8"string";
        case 288: return ns u8"feather";
        case 289: return ns u8"gunpowder";
        case 290: return ns u8"wooden_hoe";
        case 291: return ns u8"stone_hoe";
        case 292: return ns u8"iron_hoe";
        case 293: return ns u8"diamond_hoe";
        case 294: return ns u8"golden_hoe";
        case 295: return ns u8"wheat_seeds";
        case 296: return ns u8"wheat";
        case 297: return ns u8"bread";
        case 298: return ns u8"leather_helmet";
        case 299: return ns u8"leather_chestplate";
        case 300: return ns u8"leather_leggings";
        case 301: return ns u8"leather_boots";
        case 302: return ns u8"chainmail_helmet";
        case 303: return ns u8"chainmail_chestplate";
        case 304: return ns u8"chainmail_leggings";
        case 305: return ns u8"chainmail_boots";
        case 306: return ns u8"iron_helmet";
        case 307: return ns u8"iron_chestplate";
        case 308: return ns u8"iron_leggings";
        case 309: return ns u8"iron_boots";
        case 310: return ns u8"diamond_helmet";
        case 311: return ns u8"diamond_chestplate";
        case 312: return ns u8"diamond_leggings";
        case 313: return ns u8"diamond_boots";
        case 314: return ns u8"golden_helmet";
        case 315: return ns u8"golden_chestplate";
        case 316: return ns u8"golden_leggings";
        case 317: return ns u8"golden_boots";
        case 318: return ns u8"flint";
        case 319: return ns u8"porkchop";
        case 320: return ns u8"cooked_porkchop";
        case 321: return ns u8"painting";
        case 322:
            switch (DrainDamage(damage)) {
            case 1: return ns u8"enchanted_golden_apple";
            case 0:
            default:
                return ns u8"golden_apple";
            }
        case 323: return ns u8"sign";
        case 324: return ns u8"oak_door";
        case 325: return ns u8"bucket";
        case 326: return ns u8"water_bucket";
        case 327: return ns u8"lava_bucket";
        case 328: return ns u8"minecart";
        case 329: return ns u8"saddle";
        case 330: return ns u8"iron_door";
        case 331: return ns u8"redstone";
        case 332: return ns u8"snowball";
        case 333: return ns u8"oak_boat";
        case 334: return ns u8"leather";
        case 335: return ns u8"milk_bucket";
        case 336: return ns u8"brick";
        case 337: return ns u8"clay_ball";
        case 338: return ns u8"sugar_cane";
        case 339: return ns u8"paper";
        case 340: return ns u8"book";
        case 341: return ns u8"slime_ball";
        case 342: return ns u8"chest_minecart";
        case 343: return ns u8"furnace_minecart";
        case 344: return ns u8"egg";
        case 345: return ns u8"compass";
        case 346: return ns u8"fishing_rod";
        case 347: return ns u8"clock";
        case 348: return ns u8"glowstone_dust";
        case 349:
            switch (DrainDamage(damage)) {
            case 1: return ns u8"salmon";
            case 2: return ns u8"tropical_fish";
            case 3: return ns u8"pufferfish";
            case 0:
            default:
                return ns u8"cod";
            }
            break;
        case 350:
            switch (DrainDamage(damage)) {
            case 1: return ns u8"cooked_salmon";
            case 0:
            default:
                return ns u8"cooked_fish";
            }
        case 351:
            switch (DrainDamage(damage)) {
            case 1: return ns u8"red_dye";
            case 2: return ns u8"green_dye";
            case 3: return ns u8"cocoa_beans";
            case 4: return ns u8"lapis_lazuli";
            case 5: return ns u8"purple_dye";
            case 6: return ns u8"cyan_dye";
            case 7: return ns u8"light_gray_dye";
            case 8: return ns u8"gray_dye";
            case 9: return ns u8"pink_dye";
            case 10: return ns u8"lime_dye";
            case 11: return ns u8"yellow_dye";
            case 12: return ns u8"light_blue_dye";
            case 13: return ns u8"magenta_dye";
            case 14: return ns u8"orange_dye";
            case 15: return ns u8"bone_meal";
            case 0:
            default:
                return ns u8"ink_sac";
            }
        case 352: return ns u8"bone";
        case 353: return ns u8"sugar";
        case 354: return ns u8"cake";
        case 355: return ns u8"red_bed";
        case 356: return ns u8"repeater";
        case 357: return ns u8"cookie";
        case 358: return ns u8"filled_map";
        case 359: return ns u8"shears";
        case 360: return ns u8"melon_slice";
        case 361: return ns u8"pumpkin_seeds";
        case 362: return ns u8"melon_seeds";
        case 363: return ns u8"beef";
        case 364: return ns u8"cooked_beef";
        case 365: return ns u8"chicken";
        case 366: return ns u8"cooked_chicken";
        case 367: return ns u8"rotten_flesh";
        case 368: return ns u8"ender_pearl";
        case 369: return ns u8"blaze_rod";
        case 370: return ns u8"ghast_tear";
        case 371: return ns u8"gold_nugget";
        case 372: return ns u8"nether_wart";
        case 373: return ns u8"potion";
        case 374: return ns u8"glass_bottle";
        case 375: return ns u8"spider_eye";
        case 376: return ns u8"fermented_spider_eye";
        case 377: return ns u8"blaze_powder";
        case 378: return ns u8"magma_cream";
        case 379: return ns u8"brewing_stand";
        case 380: return ns u8"cauldron";
        case 381: return ns u8"ender_eye";
        case 382: return ns u8"glistering_melon_slice";
        case 383:
            switch (DrainDamage(damage)) {
            case 50: return ns u8"creeper" egg;
            case 51: return ns u8"skeleton" egg;
            case 52: return ns u8"spider" egg;
            case 54: return ns u8"zombie" egg;
            case 55: return ns u8"slime" egg;
            case 56: return ns u8"ghast" egg;
            case 57: return ns u8"zombified_piglin" egg;
            case 58: return ns u8"enderman" egg;
            case 59: return ns u8"cave_spider" egg;
            case 60: return ns u8"silverfish" egg;
            case 61: return ns u8"blaze" egg;
            case 62: return ns u8"magma_cube" egg;
            case 65: return ns u8"bat" egg;
            case 66: return ns u8"witch" egg;
            case 90: return ns u8"pig" egg;
            case 91: return ns u8"sheep" egg;
            case 92: return ns u8"cow" egg;
            case 93: return ns u8"chicken" egg;
            case 94: return ns u8"squid" egg;
            case 95: return ns u8"wolf" egg;
            case 96: return ns u8"mooshroom" egg;
            case 98: return ns u8"ocelot" egg;
            case 100: return ns u8"horse" egg;
            case 120: return ns u8"villager" egg;
            default:
                return nullopt;
            }
        case 384: return ns u8"experience_bottle";
        case 385: return ns u8"fire_charge";
        case 386: return ns u8"writable_book";
        case 387: return ns u8"written_book";
        case 388: return ns u8"emerald";
        case 389: return ns u8"item_frame";
        case 390: return ns u8"flower_pot";
        case 391: return ns u8"carrot";
        case 392: return ns u8"potato";
        case 393: return ns u8"baked_potato";
        case 394: return ns u8"poisonous_potato";
        case 395: return ns u8"map";
        case 396: return ns u8"golden_carrot";
        case 397:
            switch (DrainDamage(damage)) {
            case 1: return ns u8"wither_skeleton_skull";
            case 2: return ns u8"zombie_head";
            case 3: return ns u8"player_head";
            case 4: return ns u8"creeper_head";
            case 5: return ns u8"dragon_head";
            case 0:
            default:
                return ns u8"skeleton_skull";
            }
        case 398: return ns u8"carrot_on_a_stick";
        case 399: return ns u8"nether_star";
        case 400: return ns u8"pumpkin_pie";
        case 401: return ns u8"firework_rocket";
        case 402: return ns u8"firework_star";
        case 403: return ns u8"enchanted_book";
        case 404: return ns u8"comparator";
        case 405: return ns u8"nether_brick";
        case 406: return ns u8"quartz";
        case 407: return ns u8"tnt_minecart";
        case 408: return ns u8"hopper_minecart";
        case 409: return ns u8"prismarine_shard";
        case 410: return ns u8"prismarine_crystals";
        case 411: return ns u8"rabbit";
        case 412: return ns u8"cooked_rabbit";
        case 413: return ns u8"rabbit_stew";
        case 414: return ns u8"rabbit_foot";
        case 415: return ns u8"rabbit_hide";
        case 416: return ns u8"armor_stand";
        case 417: return ns u8"iron_horse_armor";
        case 418: return ns u8"golden_horse_armor";
        case 419: return ns u8"diamond_horse_armor";
        case 420: return ns u8"lead";
        case 421: return ns u8"name_tag";
        case 422: return ns u8"command_block_minecart";
        case 423: return ns u8"mutton";
        case 424: return ns u8"cooked_mutton";
        case 425: return ns u8"banner";
        case 426: return ns u8"end_crystal";
        case 427: return ns u8"spruce_door";
        case 428: return ns u8"birch_door";
        case 429: return ns u8"jungle_door";
        case 430: return ns u8"acacia_door";
        case 431: return ns u8"dark_oak_door";
        case 432: return ns u8"chorus_fruit";
        case 433: return ns u8"popped_chorus_fruit";
        case 434: return ns u8"beetroot";
        case 435: return ns u8"beetroot_seeds";
        case 436: return ns u8"beetroot_soup";
        case 437: return ns u8"dragon_breath";
        case 438: return ns u8"splash_potion";
        case 439: return ns u8"spectral_arrow";
        case 440: return ns u8"tipped_arrow";
        case 441: return ns u8"lingering_potion";
        case 442: return ns u8"shield";
        case 443: return ns u8"elytra";
        case 444: return ns u8"spruce_boat";
        case 445: return ns u8"birch_boat";
        case 446: return ns u8"jungle_boat";
        case 447: return ns u8"acacia_boat";
        case 448: return ns u8"dark_oak_boat";
        case 449: return ns u8"totem_of_undying";
        case 450: return ns u8"shulker_shell";
        case 452: return ns u8"iron_nugget";
        case 453: return ns u8"knowledge_book";
        case 2256: return ns u8"music_disc_13";
        case 2257: return ns u8"music_disc_cat";
        case 2258: return ns u8"music_disc_blocks";
        case 2259: return ns u8"music_disc_chirp";
        case 2260: return ns u8"music_disc_far";
        case 2261: return ns u8"music_disc_mall";
        case 2262: return ns u8"music_disc_mellohi";
        case 2263: return ns u8"music_disc_stal";
        case 2264: return ns u8"music_disc_strad";
        case 2265: return ns u8"music_disc_ward";
        case 2266: return ns u8"music_disc_11";
        case 2267: return ns u8"music_disc_wait";
        }
        return nullopt;
#undef ns
#undef egg
    }

    template<size_t N>
    static blocks::BlockId Select(uint8_t data, blocks::BlockId const table[N], blocks::BlockId fallback) {
        if (data < N) {
            return table[data];
        } else [[unlikely]] {
            return fallback;
        }
    }

    static std::shared_ptr<mcfile::je::Block const> Block(uint16_t blockId, uint8_t data) {
        using namespace std;
        using Func = function<blocks::BlockId(uint8_t, map<u8string, u8string> &)>;
        thread_local map<u8string, u8string> props;
        props.clear();
        static Func const sAir = [](uint8_t, map<u8string, u8string> &) { return blocks::minecraft::air; };
        static Func const sTable[] = {
            // 0
            sAir,
            // 1
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[7] = {
                    blocks::minecraft::stone,
                    blocks::minecraft::granite,
                    blocks::minecraft::polished_granite,
                    blocks::minecraft::diorite,
                    blocks::minecraft::polished_diorite,
                    blocks::minecraft::andesite,
                    blocks::minecraft::polished_andesite,
                };
                return Select<7>(data, sIds, blocks::minecraft::stone);
            },
            // 2
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::grass_block; },
            // 3
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[3] = {
                    blocks::minecraft::dirt,
                    blocks::minecraft::coarse_dirt,
                    blocks::minecraft::podzol,
                };
                return Select<3>(data, sIds, blocks::minecraft::dirt);
            },
            // 4
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::cobblestone; },
            // 5
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[6] = {
                    blocks::minecraft::oak_planks,
                    blocks::minecraft::spruce_planks,
                    blocks::minecraft::birch_planks,
                    blocks::minecraft::jungle_planks,
                    blocks::minecraft::acacia_planks,
                    blocks::minecraft::dark_oak_planks,
                };
                return Select<6>(data, sIds, blocks::minecraft::oak_planks);
            },
            // 6
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[6] = {
                    blocks::minecraft::oak_sapling,
                    blocks::minecraft::spruce_sapling,
                    blocks::minecraft::birch_sapling,
                    blocks::minecraft::jungle_sapling,
                    blocks::minecraft::acacia_sapling,
                    blocks::minecraft::dark_oak_sapling,
                };
                return Select<6>(data & 0x7, sIds, blocks::minecraft::oak_sapling);
            },
            // 7
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::bedrock; },
            // 8
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::water; },
            // 9
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"level"] = String::ToString(data);
                return blocks::minecraft::water;
            },
            // 10
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::lava; },
            // 11
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"level"] = String::ToString(data);
                return blocks::minecraft::lava;
            },
            // 12
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 1: return blocks::minecraft::red_sand;
                case 0:
                default:
                    return blocks::minecraft::sand;
                }
            },
            // 13
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::gravel; },
            // 14
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::gold_ore; },
            // 15
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::iron_ore; },
            // 16
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::coal_ore; },
            // 17
            [](uint8_t data, map<u8string, u8string> &props) {
                Log(data, props);
                static blocks::BlockId const sIds[4] = {
                    blocks::minecraft::oak_log,
                    blocks::minecraft::spruce_log,
                    blocks::minecraft::birch_log,
                    blocks::minecraft::jungle_log,
                };
                return Select<4>(data & 0x3, sIds, blocks::minecraft::oak_log);
            },
            // 18
            [](uint8_t data, map<u8string, u8string> &props) {
                Leaves(data, props);
                static blocks::BlockId const sIds[4] = {
                    blocks::minecraft::oak_leaves,
                    blocks::minecraft::spruce_leaves,
                    blocks::minecraft::birch_leaves,
                    blocks::minecraft::jungle_leaves,
                };
                return Select<4>(data & 0x3, sIds, blocks::minecraft::oak_leaves);
            },
            // 19
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 1: return blocks::minecraft::wet_sponge;
                case 0:
                default:
                    return blocks::minecraft::sponge;
                }
            },
            // 20
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::glass; },
            // 21
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::lapis_ore; },
            // 22
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::lapis_block; },
            // 23
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::dispenser;
            },
            // 24
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 1: return blocks::minecraft::chiseled_sandstone;
                case 2: return blocks::minecraft::cut_sandstone;
                case 0:
                default:
                    return blocks::minecraft::sandstone;
                }
            },
            // 25
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::note_block; },
            // 26
            [](uint8_t data, map<u8string, u8string> &props) {
                Bed(data, props);
                return blocks::minecraft::red_bed;
            },
            // 27
            [](uint8_t data, map<u8string, u8string> &props) {
                PoweredRail(data, props);
                return blocks::minecraft::powered_rail;
            },
            // 28
            [](uint8_t data, map<u8string, u8string> &props) {
                PoweredRail(data, props);
                return blocks::minecraft::detector_rail;
            },
            // 29
            [](uint8_t data, map<u8string, u8string> &props) {
                Piston(data, props);
                return blocks::minecraft::sticky_piston;
            },
            // 30
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::cobweb; },
            // 31
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 2: return blocks::minecraft::fern;
                case 1:
                case 0: // shrub. Legacy console edition only
                default:
                    return blocks::minecraft::grass;
                }
            },
            // 32
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::dead_bush; },
            // 33
            [](uint8_t data, map<u8string, u8string> &props) {
                Piston(data, props);
                return blocks::minecraft::piston;
            },
            // 34
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data & 7);
                props[u8"type"] = (data & 0x8) == 0x8 ? u8"sticky" : u8"normal";
                return blocks::minecraft::piston_head;
            },
            // 35
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[16] = {
                    blocks::minecraft::white_wool,
                    blocks::minecraft::orange_wool,
                    blocks::minecraft::magenta_wool,
                    blocks::minecraft::light_blue_wool,
                    blocks::minecraft::yellow_wool,
                    blocks::minecraft::lime_wool,
                    blocks::minecraft::pink_wool,
                    blocks::minecraft::gray_wool,
                    blocks::minecraft::light_gray_wool,
                    blocks::minecraft::cyan_wool,
                    blocks::minecraft::purple_wool,
                    blocks::minecraft::blue_wool,
                    blocks::minecraft::brown_wool,
                    blocks::minecraft::green_wool,
                    blocks::minecraft::red_wool,
                    blocks::minecraft::black_wool,
                };
                return Select<16>(data, sIds, blocks::minecraft::white_wool);
            },
            // 36
            sAir,
            // 37
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::dandelion; },
            // 38
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[9] = {
                    blocks::minecraft::poppy,
                    blocks::minecraft::blue_orchid,
                    blocks::minecraft::allium,
                    blocks::minecraft::azure_bluet,
                    blocks::minecraft::red_tulip,
                    blocks::minecraft::orange_tulip,
                    blocks::minecraft::white_tulip,
                    blocks::minecraft::pink_tulip,
                    blocks::minecraft::oxeye_daisy,
                };
                return Select<9>(data, sIds, blocks::minecraft::air);
            },
            // 39
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::brown_mushroom; },
            // 40
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::red_mushroom; },
            // 41
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::gold_block; },
            // 42
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::iron_block; },
            // 43
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"type"] = u8"double";
                static blocks::BlockId const sIds[8] = {
                    blocks::minecraft::smooth_stone_slab,
                    blocks::minecraft::sandstone_slab,
                    blocks::minecraft::oak_slab,
                    blocks::minecraft::cobblestone_slab,
                    blocks::minecraft::brick_slab,
                    blocks::minecraft::stone_brick_slab,
                    blocks::minecraft::nether_brick_slab,
                    blocks::minecraft::quartz_slab,
                };
                return Select<8>(data, sIds, blocks::minecraft::smooth_stone_slab);
            },
            // 44
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"type"] = ((data >> 3) & 0x1) == 0x1 ? u8"top" : u8"bottom";
                static blocks::BlockId const sIds[8] = {
                    blocks::minecraft::smooth_stone_slab,
                    blocks::minecraft::sandstone_slab,
                    blocks::minecraft::oak_slab,
                    blocks::minecraft::cobblestone_slab,
                    blocks::minecraft::brick_slab,
                    blocks::minecraft::stone_brick_slab,
                    blocks::minecraft::nether_brick_slab,
                    blocks::minecraft::quartz_slab,
                };
                return Select<8>(data & 0x7, sIds, blocks::minecraft::smooth_stone_slab);
            },
            // 45
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::bricks; },
            // 46
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::tnt; },
            // 47
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::bookshelf; },
            // 48
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::mossy_cobblestone; },
            // 49
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::obsidian; },
            // 50
            [](uint8_t data, map<u8string, u8string> &props) {
                if (data < 5) {
                    static std::u8string const facing[4] = {u8"east", u8"west", u8"south", u8"north"};
                    props[u8"facing"] = facing[std::clamp<uint8_t>(data, 1, 4) - 1];
                    return blocks::minecraft::wall_torch;
                } else {
                    return blocks::minecraft::torch;
                }
            },
            // 51
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::fire; },
            // 52
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::spawner; },
            // 53
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::oak_stairs;
            },
            // 54
            [](uint8_t data, map<u8string, u8string> &props) {
                Chest(data, props);
                return blocks::minecraft::chest;
            },
            // 55
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"power"] = String::ToString(data);
                return blocks::minecraft::redstone_wire;
            },
            // 56
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::diamond_ore; },
            // 57
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::diamond_block; },
            // 58
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::crafting_table; },
            // 59
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::wheat;
            },
            // 60
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::farmland; },
            // 61
            [](uint8_t data, map<u8string, u8string> &props) {
                // unlit furnace
                switch (data) {
                case 3: props[u8"facing"] = u8"south"; break;
                case 4: props[u8"facing"] = u8"west"; break;
                case 5: props[u8"facing"] = u8"east"; break;
                case 2:
                default:
                    props[u8"facing"] = u8"north";
                    break;
                }
                return blocks::minecraft::furnace;
            },
            // 62
            [](uint8_t data, map<u8string, u8string> &props) {
                // lit furnace
                switch (data) {
                case 3: props[u8"facing"] = u8"south"; break;
                case 4: props[u8"facing"] = u8"west"; break;
                case 5: props[u8"facing"] = u8"east"; break;
                case 2:
                default:
                    props[u8"facing"] = u8"north";
                    break;
                }
                return blocks::minecraft::furnace;
            },
            // 63
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"rotation"] = String::ToString(data);
                return blocks::minecraft::oak_sign;
            },
            // 64
            [](uint8_t data, map<u8string, u8string> &props) {
                Door(data, props);
                return blocks::minecraft::oak_door;
            },
            // 65
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 5: props[u8"facing"] = u8"east"; break;
                case 3: props[u8"facing"] = u8"south"; break;
                case 4: props[u8"facing"] = u8"west"; break;
                case 2:
                default:
                    props[u8"facing"] = u8"north";
                    break;
                }
                return blocks::minecraft::ladder;
            },
            // 66
            [](uint8_t data, map<u8string, u8string> &props) {
                Rail(data, props);
                return blocks::minecraft::rail;
            },
            // 67
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::cobblestone_stairs;
            },
            // 68
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 3:
                    props[u8"facing"] = u8"south";
                    break;
                case 4:
                    props[u8"facing"] = u8"west";
                    break;
                case 5:
                    props[u8"facing"] = u8"east";
                    break;
                case 2:
                default:
                    props[u8"facing"] = u8"north";
                    break;
                }
                return blocks::minecraft::oak_wall_sign;
            },
            // 69
            [](uint8_t data, map<u8string, u8string> &props) {
                Lever(data, props);
                return blocks::minecraft::lever;
            },
            // 70
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::stone_pressure_plate; },
            // 71
            [](uint8_t data, map<u8string, u8string> &props) {
                Door(data, props);
                return blocks::minecraft::iron_door;
            },
            // 72
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::oak_pressure_plate; },
            // 73
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::redstone_ore; },
            // 74
            [](uint8_t data, map<u8string, u8string> &props) {
                // glowing_redstone_ore
                return blocks::minecraft::redstone_ore;
            },
            // 75
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"lit"] = u8"false";
                if (data == 5) {
                    return blocks::minecraft::redstone_torch;
                } else {
                    switch (data) {
                    case 1:
                        props[u8"facing"] = u8"east";
                        break;
                    case 2:
                        props[u8"facing"] = u8"west";
                        break;
                    case 3:
                        props[u8"facing"] = u8"south";
                        break;
                    case 4:
                        props[u8"facing"] = u8"north";
                        break;
                    }
                    return blocks::minecraft::redstone_wall_torch;
                }
            },
            // 76
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"lit"] = u8"true";
                if (data == 5) {
                    return blocks::minecraft::redstone_torch;
                } else {
                    switch (data) {
                    case 1:
                        props[u8"facing"] = u8"east";
                        break;
                    case 2:
                        props[u8"facing"] = u8"west";
                        break;
                    case 3:
                        props[u8"facing"] = u8"south";
                        break;
                    case 4:
                        props[u8"facing"] = u8"north";
                        break;
                    }
                    return blocks::minecraft::redstone_wall_torch;
                }
            },
            // 77
            [](uint8_t data, map<u8string, u8string> &props) {
                Button(data, props);
                return blocks::minecraft::stone_button;
            },
            // 78
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"layers"] = String::ToString(std::clamp(data + 1, 1, 8));
                return blocks::minecraft::snow;
            },
            // 79
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::ice; },
            // 80
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::snow_block; },
            // 81
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::cactus; },
            // 82
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::clay; },
            // 83
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::sugar_cane; },
            // 84
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::jukebox; },
            // 85
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::oak_fence; },
            // 86
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingB(data);
                return blocks::minecraft::carved_pumpkin;
            },
            // 87
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::netherrack; },
            // 88
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::soul_sand; },
            // 89
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::glowstone; },
            // 90
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 2:
                    props[u8"axis"] = u8"z";
                    break;
                case 1:
                default:
                    props[u8"axis"] = u8"x";
                    break;
                }
                return blocks::minecraft::nether_portal;
            },
            // 91
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingB(data);
                return blocks::minecraft::jack_o_lantern;
            },
            // 92
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::cake; },
            // 93
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data & 0x3) {
                case 1:
                    props[u8"facing"] = u8"west";
                    break;
                case 2:
                    props[u8"facing"] = u8"north";
                    break;
                case 3:
                    props[u8"facing"] = u8"east";
                    break;
                case 0:
                default:
                    props[u8"facing"] = u8"south";
                    break;
                }
                props[u8"delay"] = String::ToString(((data & 0xc) >> 2) + 1);
                props[u8"powered"] = u8"false";
                return blocks::minecraft::repeater;
            },
            // 94
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data & 0x3) {
                case 1:
                    props[u8"facing"] = u8"west";
                    break;
                case 2:
                    props[u8"facing"] = u8"north";
                    break;
                case 3:
                    props[u8"facing"] = u8"east";
                    break;
                case 0:
                default:
                    props[u8"facing"] = u8"south";
                    break;
                }
                props[u8"delay"] = String::ToString(((data & 0xc) >> 2) + 1);
                props[u8"powered"] = u8"true";
                return blocks::minecraft::repeater;
            },
            // 95
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[16] = {
                    blocks::minecraft::white_stained_glass,
                    blocks::minecraft::orange_stained_glass,
                    blocks::minecraft::magenta_stained_glass,
                    blocks::minecraft::light_blue_stained_glass,
                    blocks::minecraft::yellow_stained_glass,
                    blocks::minecraft::lime_stained_glass,
                    blocks::minecraft::pink_stained_glass,
                    blocks::minecraft::gray_stained_glass,
                    blocks::minecraft::light_gray_stained_glass,
                    blocks::minecraft::cyan_stained_glass,
                    blocks::minecraft::purple_stained_glass,
                    blocks::minecraft::blue_stained_glass,
                    blocks::minecraft::brown_stained_glass,
                    blocks::minecraft::green_stained_glass,
                    blocks::minecraft::red_stained_glass,
                    blocks::minecraft::black_stained_glass,
                };
                return Select<16>(data, sIds, blocks::minecraft::white_stained_glass);
            },
            // 96
            [](uint8_t data, map<u8string, u8string> &props) {
                Trapdoor(data, props);
                return blocks::minecraft::oak_trapdoor;
            },
            // 97
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[6] = {
                    blocks::minecraft::infested_stone,
                    blocks::minecraft::infested_cobblestone,
                    blocks::minecraft::infested_stone_bricks,
                    blocks::minecraft::infested_mossy_stone_bricks,
                    blocks::minecraft::infested_cracked_stone_bricks,
                    blocks::minecraft::infested_chiseled_stone_bricks,
                };
                return Select<6>(data, sIds, blocks::minecraft::infested_stone);
            },
            // 98
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[4] = {
                    blocks::minecraft::stone_bricks,
                    blocks::minecraft::mossy_stone_bricks,
                    blocks::minecraft::cracked_stone_bricks,
                    blocks::minecraft::chiseled_stone_bricks,
                };
                return Select<4>(data, sIds, blocks::minecraft::stone_bricks);
            },
            // 99
            [](uint8_t data, map<u8string, u8string> &props) {
                return MushroomBlock(blocks::minecraft::brown_mushroom_block, data, props);
            },
            // 100
            [](uint8_t data, map<u8string, u8string> &props) {
                return MushroomBlock(blocks::minecraft::red_mushroom_block, data, props);
            },
            // 101
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::iron_bars; },
            // 102
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::glass_pane; },
            // 103
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::melon; },
            // 104
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::pumpkin_stem;
            },
            // 105
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::melon_stem;
            },
            // 106
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"south"] = (data & 0x1) == 0x1 ? u8"true" : u8"false";
                props[u8"west"] = (data & 0x2) == 0x2 ? u8"true" : u8"false";
                props[u8"north"] = (data & 0x4) == 0x4 ? u8"true" : u8"false";
                props[u8"east"] = (data & 0x8) == 0x8 ? u8"true" : u8"false";
                return blocks::minecraft::vine;
            },
            // 107
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::oak_fence_gate; },
            // 108
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::brick_stairs;
            },
            // 109
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::stone_brick_stairs;
            },
            // 110
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::mycelium; },
            // 111
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::lily_pad; },
            // 112
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::nether_bricks; },
            // 113
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::nether_brick_fence; },
            // 114
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::nether_brick_stairs;
            },
            // 115
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::nether_wart;
            },
            // 116
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::enchanting_table; },
            // 117
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::brewing_stand; },
            // 118
            [](uint8_t data, map<u8string, u8string> &props) {
                if (data / 2 == 0) {
                    return blocks::minecraft::cauldron;
                } else {
                    props[u8"level"] = String::ToString(data / 2);
                    return blocks::minecraft::water_cauldron;
                }
            },
            // 119
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::end_portal; },
            // 120
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data & 0x3) {
                case 1: props[u8"facing"] = u8"west"; break;
                case 2: props[u8"facing"] = u8"north"; break;
                case 3: props[u8"facing"] = u8"east"; break;
                case 0:
                default:
                    props[u8"facing"] = u8"south";
                    break;
                }
                props[u8"eye"] = data > 3 ? u8"true" : u8"false";
                return blocks::minecraft::end_portal_frame;
            },
            // 121
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::end_stone; },
            // 122
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::dragon_egg; },
            // 123
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"lit"] = u8"false";
                return blocks::minecraft::redstone_lamp;
            },
            // 124
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"lit"] = u8"true";
                return blocks::minecraft::redstone_lamp;
            },
            // 125
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"type"] = u8"double";
                static blocks::BlockId const sIds[6] = {
                    blocks::minecraft::oak_slab,
                    blocks::minecraft::spruce_slab,
                    blocks::minecraft::birch_slab,
                    blocks::minecraft::jungle_slab,
                    blocks::minecraft::acacia_slab,
                    blocks::minecraft::dark_oak_slab,
                };
                return Select<6>(data, sIds, blocks::minecraft::oak_slab);
            },
            // 126
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"type"] = data < 8 ? u8"bottom" : u8"top";
                static blocks::BlockId const sIds[6] = {
                    blocks::minecraft::oak_slab,
                    blocks::minecraft::spruce_slab,
                    blocks::minecraft::birch_slab,
                    blocks::minecraft::jungle_slab,
                    blocks::minecraft::acacia_slab,
                    blocks::minecraft::dark_oak_slab,
                };
                return Select<6>(data & 0x7, sIds, blocks::minecraft::oak_slab);
            },
            // 127
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingN2E3S0W1(data & 0x3);
                props[u8"age"] = String::ToString(data >> 2);
                return blocks::minecraft::cocoa;
            },
            // 128
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::sandstone_stairs;
            },
            // 129
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::emerald_ore; },
            // 130
            [](uint8_t data, map<u8string, u8string> &props) {
                Chest(data, props);
                return blocks::minecraft::ender_chest;
            },
            // 131
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data & 0x3) {
                case 1:
                    props[u8"facing"] = u8"west";
                    break;
                case 2:
                    props[u8"facing"] = u8"north";
                    break;
                case 3:
                    props[u8"facing"] = u8"east";
                    break;
                case 0:
                default:
                    props[u8"facing"] = u8"south";
                    break;
                }
                props[u8"attached"] = (data & 0x4) == 0x4 ? u8"true" : u8"false";
                props[u8"powered"] = (data & 0x8) == 0x8 ? u8"true" : u8"false";
                return blocks::minecraft::tripwire_hook;
            },
            // 132
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::tripwire; },
            // 133
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::emerald_block; },
            // 134
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::spruce_stairs;
            },
            // 135
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::birch_stairs;
            },
            // 136
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::jungle_stairs;
            },
            // 137
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::command_block; },
            // 138
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::beacon; },
            // 139
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 1: return blocks::minecraft::mossy_cobblestone_wall; break;
                case 0:
                default:
                    return blocks::minecraft::cobblestone_wall;
                }
            },
            // 140
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::flower_pot; },
            // 141
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::carrots;
            },
            // 142
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::potatoes;
            },
            // 143
            [](uint8_t data, map<u8string, u8string> &props) {
                Button(data, props);
                return blocks::minecraft::oak_button;
            },
            // 144
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 1: return blocks::minecraft::skeleton_skull; break;
                case 3:
                    props[u8"facing"] = u8"south";
                    return blocks::minecraft::skeleton_wall_skull;
                case 4:
                    props[u8"facing"] = u8"west";
                    return blocks::minecraft::skeleton_wall_skull;
                case 2:
                    props[u8"facing"] = u8"north";
                    return blocks::minecraft::skeleton_wall_skull;
                case 5:
                    props[u8"facing"] = u8"east";
                    return blocks::minecraft::skeleton_wall_skull;
                default:
                    return blocks::minecraft::air;
                }
            },
            // 145
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data & 0x3) {
                case 3: props[u8"facing"] = u8"east"; break;
                case 1: props[u8"facing"] = u8"west"; break;
                case 2: props[u8"facing"] = u8"north"; break;
                case 0:
                default:
                    props[u8"facing"] = u8"south";
                    break;
                }
                switch (data >> 2) {
                case 1: return blocks::minecraft::chipped_anvil;
                case 2: return blocks::minecraft::damaged_anvil;
                case 0:
                default:
                    return blocks::minecraft::anvil;
                }
            },
            // 146
            [](uint8_t data, map<u8string, u8string> &props) {
                Chest(data, props);
                return blocks::minecraft::trapped_chest;
            },
            // 147
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::light_weighted_pressure_plate; },
            // 148
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::heavy_weighted_pressure_plate; },
            // 149
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingN2E3S0W1(data & 0x3);
                props[u8"powered"] = u8"false";
                props[u8"mode"] = (data & 0x4) == 0x4 ? u8"subtract" : u8"compare";
                return blocks::minecraft::comparator;
            },
            // 150
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingN2E3S0W1(data & 0x3);
                props[u8"powered"] = u8"true";
                props[u8"mode"] = (data & 0x4) == 0x4 ? u8"subtract" : u8"compare";
                return blocks::minecraft::comparator;
            },
            // 151
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"inverted"] = u8"false";
                return blocks::minecraft::daylight_detector;
            },
            // 152
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::redstone_block; },
            // 153
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::nether_quartz_ore; },
            // 154
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data & 0x7) {
                case 2:
                    props[u8"facing"] = u8"north";
                    break;
                case 3:
                    props[u8"facing"] = u8"south";
                    break;
                case 4:
                    props[u8"facing"] = u8"west";
                    break;
                case 5:
                    props[u8"facing"] = u8"east";
                    break;
                case 0:
                default:
                    props[u8"facing"] = u8"down";
                    break;
                }
                props[u8"enabled"] = (data & 0x8) == 0x8 ? u8"false" : u8"true";
                return blocks::minecraft::hopper;
            },
            // 155
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 1:
                    return blocks::minecraft::chiseled_quartz_block;
                case 3:
                    props[u8"axis"] = u8"x";
                    return blocks::minecraft::quartz_pillar;
                case 4:
                    props[u8"axis"] = u8"z";
                    return blocks::minecraft::quartz_pillar;
                case 2:
                    props[u8"axis"] = u8"y";
                    return blocks::minecraft::quartz_pillar;
                case 0:
                default:
                    return blocks::minecraft::quartz_block;
                }
            },
            // 156
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::quartz_stairs;
            },
            // 157
            [](uint8_t data, map<u8string, u8string> &props) {
                PoweredRail(data, props);
                return blocks::minecraft::activator_rail;
            },
            // 158
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::dropper;
            },
            // 159
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[16] = {
                    blocks::minecraft::white_terracotta,
                    blocks::minecraft::orange_terracotta,
                    blocks::minecraft::magenta_terracotta,
                    blocks::minecraft::light_blue_terracotta,
                    blocks::minecraft::yellow_terracotta,
                    blocks::minecraft::lime_terracotta,
                    blocks::minecraft::pink_terracotta,
                    blocks::minecraft::gray_terracotta,
                    blocks::minecraft::light_gray_terracotta,
                    blocks::minecraft::cyan_terracotta,
                    blocks::minecraft::purple_terracotta,
                    blocks::minecraft::blue_terracotta,
                    blocks::minecraft::brown_terracotta,
                    blocks::minecraft::green_terracotta,
                    blocks::minecraft::red_terracotta,
                    blocks::minecraft::black_terracotta,
                };
                return Select<16>(data, sIds, blocks::minecraft::white_terracotta);
            },
            // 160
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[16] = {
                    blocks::minecraft::white_stained_glass_pane,
                    blocks::minecraft::orange_stained_glass_pane,
                    blocks::minecraft::magenta_stained_glass_pane,
                    blocks::minecraft::light_blue_stained_glass_pane,
                    blocks::minecraft::yellow_stained_glass_pane,
                    blocks::minecraft::lime_stained_glass_pane,
                    blocks::minecraft::pink_stained_glass_pane,
                    blocks::minecraft::gray_stained_glass_pane,
                    blocks::minecraft::light_gray_stained_glass_pane,
                    blocks::minecraft::cyan_stained_glass_pane,
                    blocks::minecraft::purple_stained_glass_pane,
                    blocks::minecraft::blue_stained_glass_pane,
                    blocks::minecraft::brown_stained_glass_pane,
                    blocks::minecraft::green_stained_glass_pane,
                    blocks::minecraft::red_stained_glass_pane,
                    blocks::minecraft::black_stained_glass_pane,
                };
                return Select<16>(data, sIds, blocks::minecraft::white_stained_glass_pane);
            },
            // 161
            [](uint8_t data, map<u8string, u8string> &props) {
                Leaves(data, props);
                switch (data & 0x3) {
                case 1: return blocks::minecraft::dark_oak_leaves;
                case 0:
                default:
                    return blocks::minecraft::acacia_leaves;
                }
            },
            // 162
            [](uint8_t data, map<u8string, u8string> &props) {
                Log(data, props);
                switch (data & 0x3) {
                case 1: return blocks::minecraft::dark_oak_log;
                case 0:
                default:
                    return blocks::minecraft::acacia_log;
                }
            },
            // 163
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::acacia_stairs;
            },
            // 164
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::dark_oak_stairs;
            },
            // 165
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::slime_block; },
            // 166
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::barrier; },
            // 167
            [](uint8_t data, map<u8string, u8string> &props) {
                Trapdoor(data, props);
                return blocks::minecraft::iron_trapdoor;
            },
            // 168
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[3] = {
                    blocks::minecraft::prismarine,
                    blocks::minecraft::prismarine_bricks,
                    blocks::minecraft::dark_prismarine,
                };
                return Select<3>(data, sIds, blocks::minecraft::prismarine);
            },
            // 169
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::sea_lantern; },
            // 170
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"axis"] = Axis(data >> 2);
                return blocks::minecraft::hay_block;
            },
            // 171
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[16] = {
                    blocks::minecraft::white_carpet,
                    blocks::minecraft::orange_carpet,
                    blocks::minecraft::magenta_carpet,
                    blocks::minecraft::light_blue_carpet,
                    blocks::minecraft::yellow_carpet,
                    blocks::minecraft::lime_carpet,
                    blocks::minecraft::pink_carpet,
                    blocks::minecraft::gray_carpet,
                    blocks::minecraft::light_gray_carpet,
                    blocks::minecraft::cyan_carpet,
                    blocks::minecraft::purple_carpet,
                    blocks::minecraft::blue_carpet,
                    blocks::minecraft::brown_carpet,
                    blocks::minecraft::green_carpet,
                    blocks::minecraft::red_carpet,
                    blocks::minecraft::black_carpet,
                };
                return Select<16>(data, sIds, blocks::minecraft::white_carpet);
            },
            // 172
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::terracotta; },
            // 173
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::coal_block; },
            // 174
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::packed_ice; },
            // 175
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[6] = {
                    blocks::minecraft::sunflower,
                    blocks::minecraft::lilac,
                    blocks::minecraft::tall_grass,
                    blocks::minecraft::large_fern,
                    blocks::minecraft::rose_bush,
                    blocks::minecraft::peony,
                };
                return Select<6>(data, sIds, blocks::minecraft::sunflower);
            },
            // 176
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"rotation"] = String::ToString(data);
                return blocks::minecraft::white_banner;
            },
            // 177
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 3:
                    props[u8"facing"] = u8"south";
                    break;
                case 4:
                    props[u8"facing"] = u8"west";
                    break;
                case 5:
                    props[u8"facing"] = u8"east";
                    break;
                case 2:
                default:
                    props[u8"facing"] = u8"north";
                    break;
                }
                return blocks::minecraft::white_wall_banner;
            },
            // 178
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"inverted"] = u8"true";
                return blocks::minecraft::daylight_detector;
            },
            // 179
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[3] = {
                    blocks::minecraft::red_sandstone,
                    blocks::minecraft::chiseled_red_sandstone,
                    blocks::minecraft::cut_red_sandstone,
                };
                return Select<3>(data, sIds, blocks::minecraft::red_sandstone);
            },
            // 180
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::red_sandstone_stairs;
            },
            // 181
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"type"] = u8"double";
                return blocks::minecraft::red_sandstone_slab;
            },
            // 182
            [](uint8_t data, map<u8string, u8string> &props) {
                switch (data) {
                case 8:
                    props[u8"type"] = u8"top";
                    return blocks::minecraft::red_sandstone_slab;
                case 0:
                default:
                    props[u8"type"] = u8"bottom";
                    return blocks::minecraft::red_sandstone_slab;
                }
            },
            // 183
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::spruce_fence_gate; },
            // 184
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::birch_fence_gate; },
            // 185
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::jungle_fence_gate; },
            // 186
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::dark_oak_fence_gate; },
            // 187
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::acacia_fence_gate; },
            // 188
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::spruce_fence; },
            // 189
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::birch_fence; },
            // 190
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::jungle_fence; },
            // 191
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::dark_oak_fence; },
            // 192
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::acacia_fence; },
            // 193
            [](uint8_t data, map<u8string, u8string> &props) {
                Door(data, props);
                return blocks::minecraft::spruce_door;
            },
            // 194
            [](uint8_t data, map<u8string, u8string> &props) {
                Door(data, props);
                return blocks::minecraft::birch_door;
            },
            // 195
            [](uint8_t data, map<u8string, u8string> &props) {
                Door(data, props);
                return blocks::minecraft::jungle_door;
            },
            // 196
            [](uint8_t data, map<u8string, u8string> &props) {
                Door(data, props);
                return blocks::minecraft::acacia_door;
            },
            // 197
            [](uint8_t data, map<u8string, u8string> &props) {
                Door(data, props);
                return blocks::minecraft::dark_oak_door;
            },
            // 198
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::end_rod;
            },
            // 199
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::chorus_plant; },
            // 200
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::chorus_flower;
            },
            // 201
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::purpur_block; },
            // 202
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"axis"] = Axis(data >> 2);
                return blocks::minecraft::purpur_pillar;
            },
            // 203
            [](uint8_t data, map<u8string, u8string> &props) {
                Stairs(data, props);
                return blocks::minecraft::purpur_stairs;
            },
            // 204
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"type"] = u8"double";
                return blocks::minecraft::purpur_slab;
            },
            // 205
            [](uint8_t data, map<u8string, u8string> &props) {
                if (data == 8) {
                    props[u8"type"] = u8"top";
                } else {
                    props[u8"type"] = u8"bottom";
                }
                return blocks::minecraft::purpur_slab;
            },
            // 206
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::end_stone_bricks; },
            // 207
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"age"] = String::ToString(data);
                return blocks::minecraft::beetroots;
            },
            // 208
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::dirt_path; },
            // 209
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::end_gateway; },
            // 210
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::repeating_command_block; },
            // 211
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::chain_command_block; },
            // 212
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::frosted_ice; },
            // 213
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::magma_block; },
            // 214
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::nether_wart_block; },
            // 215
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::red_nether_bricks; },
            // 216
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"axis"] = Axis(data >> 2);
                return blocks::minecraft::bone_block;
            },
            // 217
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::structure_void; },
            // 218
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::observer;
            },
            // 219
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::white_shulker_box;
            },
            // 220
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::orange_shulker_box;
            },
            // 221
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::magenta_shulker_box;
            },
            // 222
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::light_blue_shulker_box;
            },
            // 223
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::yellow_shulker_box;
            },
            // 224
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::lime_shulker_box;
            },
            // 225
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::pink_shulker_box;
            },
            // 226
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::gray_shulker_box;
            },
            // 227
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::light_gray_shulker_box;
            },
            // 228
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::cyan_shulker_box;
            },
            // 229
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::shulker_box;
            },
            // 230
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::blue_shulker_box;
            },
            // 231
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::brown_shulker_box;
            },
            // 232
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::green_shulker_box;
            },
            // 233
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::red_shulker_box;
            },
            // 234
            [](uint8_t data, map<u8string, u8string> &props) {
                props[u8"facing"] = FacingA(data);
                return blocks::minecraft::black_shulker_box;
            },
            // 235
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::white_glazed_terracotta;
            },
            // 236
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::orange_glazed_terracotta;
            },
            // 237
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::magenta_glazed_terracotta;
            },
            // 238
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::light_blue_glazed_terracotta;
            },
            // 239
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::yellow_glazed_terracotta;
            },
            // 240
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::lime_glazed_terracotta;
            },
            // 241
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::pink_glazed_terracotta;
            },
            // 242
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::gray_glazed_terracotta;
            },
            // 243
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::light_gray_glazed_terracotta;
            },
            // 244
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::cyan_glazed_terracotta;
            },
            // 245
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::purple_glazed_terracotta;
            },
            // 246
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::blue_glazed_terracotta;
            },
            // 247
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::brown_glazed_terracotta;
            },
            // 248
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::green_glazed_terracotta;
            },
            // 249
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::red_glazed_terracotta;
            },
            // 250
            [](uint8_t data, map<u8string, u8string> &props) {
                GlazedTerracotta(data, props);
                return blocks::minecraft::black_glazed_terracotta;
            },
            // 251
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[16] = {
                    blocks::minecraft::white_concrete,
                    blocks::minecraft::orange_concrete,
                    blocks::minecraft::magenta_concrete,
                    blocks::minecraft::light_blue_concrete,
                    blocks::minecraft::yellow_concrete,
                    blocks::minecraft::lime_concrete,
                    blocks::minecraft::pink_concrete,
                    blocks::minecraft::gray_concrete,
                    blocks::minecraft::light_gray_concrete,
                    blocks::minecraft::cyan_concrete,
                    blocks::minecraft::purple_concrete,
                    blocks::minecraft::blue_concrete,
                    blocks::minecraft::brown_concrete,
                    blocks::minecraft::green_concrete,
                    blocks::minecraft::red_concrete,
                    blocks::minecraft::black_concrete};
                return Select<16>(data, sIds, blocks::minecraft::white_concrete);
            },
            // 252
            [](uint8_t data, map<u8string, u8string> &props) {
                static blocks::BlockId const sIds[16] = {
                    blocks::minecraft::white_concrete_powder,
                    blocks::minecraft::orange_concrete_powder,
                    blocks::minecraft::magenta_concrete_powder,
                    blocks::minecraft::light_blue_concrete_powder,
                    blocks::minecraft::yellow_concrete_powder,
                    blocks::minecraft::lime_concrete_powder,
                    blocks::minecraft::pink_concrete_powder,
                    blocks::minecraft::gray_concrete_powder,
                    blocks::minecraft::light_gray_concrete_powder,
                    blocks::minecraft::cyan_concrete_powder,
                    blocks::minecraft::purple_concrete_powder,
                    blocks::minecraft::blue_concrete_powder,
                    blocks::minecraft::brown_concrete_powder,
                    blocks::minecraft::green_concrete_powder,
                    blocks::minecraft::red_concrete_powder,
                    blocks::minecraft::black_concrete_powder};
                return Select<16>(data, sIds, blocks::minecraft::white_concrete_powder);
            },
            // 253
            sAir,
            // 254
            sAir,
            // 255
            [](uint8_t data, map<u8string, u8string> &props) { return blocks::minecraft::structure_block; },
        };
        static_assert(sizeof(sTable) == 256 * sizeof(Func));
        auto id = sTable[0xff & blockId](data, props);
        return std::make_shared<mcfile::je::Block>(id, props);
    }

    static inline uint8_t Nibble4(std::vector<uint8_t> const &arr, int index) {
        return index % 2 == 0 ? arr[index / 2] & 0x0F : (arr[index / 2] >> 4) & 0x0F;
    }

    static void Stairs(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        static std::u8string const facing[4] = {u8"east", u8"west", u8"south", u8"north"};
        props[u8"facing"] = facing[std::clamp(data & 0x3, 0, 3)];
        static std::u8string const half[2] = {u8"bottm", u8"top"};
        props[u8"half"] = half[std::clamp(data & 0x4, 0, 1)];
    }

    static void Door(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        static std::u8string const facing[4] = {u8"east", u8"south", u8"west", u8"north"};
        static std::u8string const hinge[2] = {u8"left", u8"right"};
        static std::u8string const open[2] = {u8"false", u8"true"};
        static std::u8string const powered[2] = {u8"false", u8"true"};
        if (data < 8) {
            props[u8"half"] = u8"lower";
            props[u8"facing"] = facing[std::clamp(data & 0x3, 0, 3)];
            props[u8"open"] = open[std::clamp((data >> 2) & 0x1, 0, 1)];
        } else {
            props[u8"half"] = u8"upper";
            props[u8"hinge"] = hinge[std::clamp(data & 0x1, 0, 1)];
            props[u8"powered"] = powered[std::clamp((data >> 1) & 0x1, 0, 1)];
        }
    }

    static std::u8string Axis(uint8_t v) {
        static std::u8string const axis[3] = {u8"y", u8"x", u8"z"};
        return axis[std::clamp<uint8_t>(v, 0, 2)];
    }

    static void Log(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        props[u8"axis"] = Axis((data >> 2) & 0x3);
    }

    static void Leaves(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        if (((data >> 2) & 0x1) == 0x1) {
            props[u8"persistent"] = u8"true";
        } else {
            props[u8"persistent"] = u8"false";
        }
    }

    static void Bed(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        switch (data & 0x3) {
        case 2:
            props[u8"facing"] = u8"north";
            break;
        case 3:
            props[u8"facing"] = u8"east";
            break;
        case 1:
            props[u8"facing"] = u8"west";
            break;
        case 0:
        default:
            props[u8"facing"] = u8"south";
            break;
        }
        props[u8"occupied"] = ((data >> 2) & 0x1) == 0x1 ? u8"true" : u8"false";
        props[u8"part"] = ((data >> 3) & 0x1) == 0x1 ? u8"head" : u8"foot";
    }

    static void Rail(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        static std::u8string const shape[10] = {
            u8"north_south",     // 0
            u8"east_west",       // 1
            u8"ascending_east",  // 2
            u8"ascending_west",  // 3
            u8"ascending_north", // 4
            u8"ascending_south", // 5
            u8"south_east",      // 6
            u8"south_west",      // 7
            u8"north_west",      // 8
            u8"north_east",      // 9
        };
        props[u8"shape"] = shape[std::clamp<uint8_t>(data, 0, 9)];
    }

    static void PoweredRail(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        static std::u8string const shape[6] = {
            u8"north_south",     // 0
            u8"east_west",       // 1
            u8"ascending_east",  // 2
            u8"ascending_west",  // 3
            u8"ascending_north", // 4
            u8"ascending_south"  // 5
        };
        props[u8"shape"] = shape[std::clamp(data & 0x7, 0, 5)];
        props[u8"powered"] = ((data >> 3) & 0x1) == 0x1 ? u8"true" : u8"false";
    }

    static std::u8string FacingA(uint8_t data) {
        static std::u8string const facing[6] = {u8"down", u8"up", u8"north", u8"south", u8"west", u8"east"};
        return facing[std::clamp<uint8_t>(data, 0, 5)];
    }

    static void Piston(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        auto facing = FacingA(data & 0x7);
        props[u8"facing"] = facing;
        props[u8"extended"] = ((data >> 3) & 0x1) == 0x1 ? u8"true" : u8"false";
    }

    static std::u8string FacingB(uint8_t data) {
        static std::u8string const facing[4] = {u8"south", u8"west", u8"north", u8"east"};
        return facing[std::clamp<uint8_t>(data, 0, 3)];
    }

    static void Chest(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        switch (data) {
        case 3: props[u8"facing"] = u8"south"; break;
        case 4: props[u8"facing"] = u8"west"; break;
        case 5: props[u8"facing"] = u8"east"; break;
        case 2:
        default:
            props[u8"facing"] = u8"north";
            break;
        }
    }

    static void GlazedTerracotta(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        switch (data) {
        case 0: props[u8"facing"] = u8"south"; break;
        case 1: props[u8"facing"] = u8"west"; break;
        case 2: props[u8"facing"] = u8"north"; break;
        case 3: props[u8"facing"] = u8"east"; break;
        }
    }

    static void Trapdoor(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        switch (data & 0x3) {
        case 1: props[u8"facing"] = u8"south"; break;
        case 2: props[u8"facing"] = u8"west"; break;
        case 3: props[u8"facing"] = u8"east"; break;
        default:
        case 0:
            props[u8"facing"] = u8"north";
            break;
        }
        props[u8"half"] = (data & 0x8) == 0x8 ? u8"top" : u8"bottom";
        props[u8"open"] = (data & 0x4) == 0x4 ? u8"true" : u8"fasle";
    }

    static void Button(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        switch (data & 0x7) {
        case 0:
            props[u8"face"] = u8"ceiling";
            break;
        case 1:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"east";
            break;
        case 2:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"west";
            break;
        case 3:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"south";
            break;
        case 4:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"north";
            break;
        case 5:
            props[u8"face"] = u8"floor";
            break;
        }
        props[u8"powered"] = (data & 0x8) == 0x8 ? u8"true" : u8"false";
    }

    static void Lever(uint8_t data, std::map<std::u8string, std::u8string> &props) {
        switch (data & 0x7) {
        case 0:
            props[u8"face"] = u8"ceiling";
            props[u8"facing"] = u8"west";
            break;
        case 1:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"east";
            break;
        case 2:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"west";
            break;
        case 3:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"south";
            break;
        case 4:
            props[u8"face"] = u8"wall";
            props[u8"facing"] = u8"north";
            break;
        case 5:
            props[u8"face"] = u8"floor";
            props[u8"facing"] = u8"north";
            break;
        case 6:
            props[u8"face"] = u8"floor";
            props[u8"facing"] = u8"west";
            break;
        case 7:
            props[u8"face"] = u8"ceiling";
            props[u8"facing"] = u8"north";
            break;
        }
        props[u8"powered"] = (data & 0x8) == 0x8 ? u8"true" : u8"false";
    }

    static std::u8string FacingN2E3S0W1(uint8_t data) {
        switch (data) {
        case 1: return u8"west";
        case 2: return u8"north";
        case 3: return u8"east";
        case 0:
        default:
            return u8"south";
        }
    }

    static mcfile::blocks::BlockId MushroomBlock(mcfile::blocks::BlockId id, uint8_t data, std::map<std::u8string, std::u8string> &props) {
        switch (data) {
        case 1:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"false";
            props[u8"north"] = u8"true";
            props[u8"south"] = u8"false";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"true";
            return id;
        case 2:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"false";
            props[u8"north"] = u8"true";
            props[u8"south"] = u8"false";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"false";
            return id;
        case 3:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"true";
            props[u8"north"] = u8"true";
            props[u8"south"] = u8"false";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"false";
            return id;
        case 4:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"false";
            props[u8"north"] = u8"false";
            props[u8"south"] = u8"false";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"true";
            return id;
        case 5:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"false";
            props[u8"north"] = u8"false";
            props[u8"south"] = u8"false";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"false";
            return id;
        case 6:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"true";
            props[u8"north"] = u8"false";
            props[u8"south"] = u8"false";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"false";
            return id;
        case 7:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"false";
            props[u8"north"] = u8"false";
            props[u8"south"] = u8"true";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"true";
            return id;
        case 8:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"false";
            props[u8"north"] = u8"false";
            props[u8"south"] = u8"true";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"false";
            return id;
        case 9:
            props[u8"down"] = u8"false";
            props[u8"east"] = u8"true";
            props[u8"north"] = u8"false";
            props[u8"south"] = u8"true";
            props[u8"up"] = u8"true";
            props[u8"west"] = u8"false";
            return id;
        case 10:
            props[u8"down"] = u8"false";
            props[u8"up"] = u8"false";
            props[u8"east"] = u8"true";
            props[u8"south"] = u8"true";
            props[u8"west"] = u8"true";
            props[u8"north"] = u8"true";
            return blocks::minecraft::mushroom_stem;
        case 11:
            props[u8"down"] = u8"true";
            props[u8"up"] = u8"true";
            props[u8"east"] = u8"true";
            props[u8"south"] = u8"true";
            props[u8"west"] = u8"true";
            props[u8"north"] = u8"true";
            return id;
        case 0:
        default:
            props[u8"down"] = u8"false";
            props[u8"up"] = u8"false";
            props[u8"east"] = u8"false";
            props[u8"south"] = u8"false";
            props[u8"west"] = u8"false";
            props[u8"north"] = u8"false";
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
