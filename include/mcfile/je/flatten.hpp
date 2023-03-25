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
            case 0:
            default:
                return ns u8"skeleton_skull";
            }
        case 398: return ns u8"carrot_on_a_stick";
        case 399: return ns u8"nether_star";
        case 400: return ns u8"pumpkin_pie";
        case 402: return ns u8"firework_star";
        case 403: return ns u8"enchanted_book";
        case 404: return ns u8"comparator";
        case 405: return ns u8"nether_brick";
        case 406: return ns u8"quartz";
        case 407: return ns u8"tnt_minecart";
        case 408: return ns u8"hopper_minecart";
        case 417: return ns u8"iron_horse_armor";
        case 418: return ns u8"golden_horse_armor";
        case 419: return ns u8"diamond_horse_armor";
        case 420: return ns u8"lead";
        case 421: return ns u8"name_tag";
        case 2256: return ns u8"music_disc_13";
        case 2257: return ns u8"music_disc_cat";
        case 2258: return ns u8"music_disc_blocks";
        case 2259: return ns u8"music_disc_chirp";
        case 2260: return ns u8"music_disc_far";
        case 2261: return ns u8"music_disc_mall";
        case 2262: return ns u8"music_disc_mellohi";
        case 2267: return ns u8"music_disc_wait";
        case 2263: return ns u8"music_disc_stal";
        case 2264: return ns u8"music_disc_strad";
        case 2265: return ns u8"music_disc_ward";
        case 2266: return ns u8"music_disc_11";
        }
        return nullopt;
#undef ns
#undef egg
    }

    static std::shared_ptr<mcfile::je::Block const> Block(uint16_t blockId, uint8_t data) {
        blocks::BlockId id = blocks::minecraft::air;
        std::map<std::u8string, std::u8string> props;
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
            props[u8"level"] = String::ToString(data);
            break;
        case 10: id = blocks::minecraft::lava; break; //TODO: flowing_lava
        case 11:
            id = blocks::minecraft::lava;
            props[u8"level"] = String::ToString(data);
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
            case 1: id = blocks::minecraft::spruce_leaves; break;
            case 2: id = blocks::minecraft::birch_leaves; break;
            case 3: id = blocks::minecraft::jungle_leaves; break;
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
            props[u8"facing"] = FacingA(data);
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
            props[u8"facing"] = FacingA(data & 7);
            props[u8"type"] = (data & 0x8) == 0x8 ? u8"sticky" : u8"normal";
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
            case 0: id = blocks::minecraft::poppy; break;
            case 1: id = blocks::minecraft::blue_orchid; break;
            case 2: id = blocks::minecraft::allium; break;
            case 3: id = blocks::minecraft::azure_bluet; break;
            case 4: id = blocks::minecraft::red_tulip; break;
            case 5: id = blocks::minecraft::orange_tulip; break;
            case 6: id = blocks::minecraft::white_tulip; break;
            case 7: id = blocks::minecraft::pink_tulip; break;
            case 8: id = blocks::minecraft::oxeye_daisy; break;
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
            props[u8"type"] = u8"double";
            break;
        case 44:
            switch (data & 0x7) {
            case 1: id = blocks::minecraft::sandstone_slab; break;
            case 2: id = blocks::minecraft::oak_slab; break;
            case 3: id = blocks::minecraft::cobblestone_slab; break;
            case 4: id = blocks::minecraft::brick_slab; break;
            case 5: id = blocks::minecraft::stone_brick_slab; break;
            case 6: id = blocks::minecraft::nether_brick_slab; break;
            case 7: id = blocks::minecraft::quartz_slab; break;
            case 0:
            default:
                id = id = blocks::minecraft::smooth_stone_slab;
                break;
            }
            props[u8"type"] = ((data >> 3) & 0x1) == 0x1 ? u8"top" : u8"bottom";
            break;
        case 45: id = blocks::minecraft::bricks; break;
        case 46: id = blocks::minecraft::tnt; break;
        case 47: id = blocks::minecraft::bookshelf; break;
        case 48: id = blocks::minecraft::mossy_cobblestone; break;
        case 49: id = blocks::minecraft::obsidian; break;
        case 50:
            if (data < 5) {
                static std::u8string const facing[4] = {u8"east", u8"west", u8"south", u8"north"};
                props[u8"facing"] = facing[std::clamp<uint8_t>(data, 1, 4) - 1];
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
            props[u8"power"] = String::ToString(data);
            break;
        case 56: id = blocks::minecraft::diamond_ore; break;
        case 57: id = blocks::minecraft::diamond_block; break;
        case 58: id = blocks::minecraft::crafting_table; break;
        case 59:
            id = blocks::minecraft::wheat;
            props[u8"age"] = String::ToString(data);
            break;
        case 60: id = blocks::minecraft::farmland; break;
        case 61:
        case 62: // lit
            id = blocks::minecraft::furnace;
            switch (data) {
            case 3: props[u8"facing"] = u8"south"; break;
            case 4: props[u8"facing"] = u8"west"; break;
            case 5: props[u8"facing"] = u8"east"; break;
            case 2:
            default:
                props[u8"facing"] = u8"north";
                break;
            }
            break;
        case 63:
            id = blocks::minecraft::oak_sign;
            props[u8"rotation"] = String::ToString(data);
            break;
        case 64:
            id = blocks::minecraft::oak_door;
            Door(data, props);
            break;
        case 65:
            id = blocks::minecraft::ladder;
            switch (data) {
            case 5: props[u8"facing"] = u8"east"; break;
            case 3: props[u8"facing"] = u8"south"; break;
            case 4: props[u8"facing"] = u8"west"; break;
            case 2:
            default:
                props[u8"facing"] = u8"north";
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
            }
            props[u8"lit"] = blockId == 76 ? u8"true" : u8"false";
            break;
        case 77:
            id = blocks::minecraft::stone_button;
            Button(data, props);
            break;
        case 78:
            id = blocks::minecraft::snow;
            props[u8"layers"] = String::ToString(std::clamp(data + 1, 1, 8));
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
            props[u8"facing"] = FacingB(data);
            break;
        case 87: id = blocks::minecraft::netherrack; break;
        case 88: id = blocks::minecraft::soul_sand; break;
        case 89: id = blocks::minecraft::glowstone; break;
        case 90:
            id = blocks::minecraft::nether_portal;
            switch (data) {
            case 2:
                props[u8"axis"] = u8"z";
                break;
            case 1:
            default:
                props[u8"axis"] = u8"x";
                break;
            }
            break;
        case 91:
            id = blocks::minecraft::jack_o_lantern;
            props[u8"facing"] = FacingB(data);
            break;
        case 92: id = blocks::minecraft::cake; break;
        case 93:
        case 94:
            id = blocks::minecraft::repeater;
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
            props[u8"powered"] = blockId == 94 ? u8"true" : u8"false";
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
            props[u8"age"] = String::ToString(data);
            break;
        case 105:
            id = blocks::minecraft::melon_stem;
            props[u8"age"] = String::ToString(data);
            break;
        case 106:
            id = blocks::minecraft::vine;
            props[u8"south"] = (data & 0x1) == 0x1 ? u8"true" : u8"false";
            props[u8"west"] = (data & 0x2) == 0x2 ? u8"true" : u8"false";
            props[u8"north"] = (data & 0x4) == 0x4 ? u8"true" : u8"false";
            props[u8"east"] = (data & 0x8) == 0x8 ? u8"true" : u8"false";
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
            props[u8"age"] = String::ToString(data);
            break;
        case 116: id = blocks::minecraft::enchanting_table; break;
        case 117: id = blocks::minecraft::brewing_stand; break;
        case 118:
            if (data / 2 == 0) {
                id = blocks::minecraft::cauldron;
            } else {
                id = blocks::minecraft::water_cauldron;
                props[u8"level"] = String::ToString(data / 2);
            }
            break;
        case 119: id = blocks::minecraft::end_portal; break;
        case 120:
            id = blocks::minecraft::end_portal_frame;
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
            break;
        case 121: id = blocks::minecraft::end_stone; break;
        case 122: id = blocks::minecraft::dragon_egg; break;
        case 123:
            id = blocks::minecraft::redstone_lamp;
            props[u8"lit"] = u8"false";
            break;
        case 124:
            id = blocks::minecraft::redstone_lamp;
            props[u8"lit"] = u8"true";
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
            props[u8"type"] = u8"double";
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
            props[u8"type"] = data < 8 ? u8"bottom" : u8"top";
            break;
        case 127:
            id = blocks::minecraft::cocoa;
            props[u8"facing"] = FacingN2E3S0W1(data & 0x3);
            props[u8"age"] = String::ToString(data >> 2);
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
            props[u8"age"] = String::ToString(data);
            break;
        case 142:
            id = blocks::minecraft::potatoes;
            props[u8"age"] = String::ToString(data);
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
                props[u8"facing"] = u8"south";
                break;
            case 4:
                id = blocks::minecraft::skeleton_wall_skull;
                props[u8"facing"] = u8"west";
                break;
            case 2:
                id = blocks::minecraft::skeleton_wall_skull;
                props[u8"facing"] = u8"north";
                break;
            case 5:
                id = blocks::minecraft::skeleton_wall_skull;
                props[u8"facing"] = u8"east";
                break;
            }
            break;
        case 145:
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
            props[u8"facing"] = FacingN2E3S0W1(data & 0x3);
            props[u8"powered"] = blockId == 150 ? u8"true" : u8"false";
            props[u8"mode"] = (data & 0x4) == 0x4 ? u8"subtract" : u8"compare";
            break;
        case 151:
            id = blocks::minecraft::daylight_detector;
            props[u8"inverted"] = u8"false";
            break;
        case 152: id = blocks::minecraft::redstone_block; break;
        case 153: id = blocks::minecraft::nether_quartz_ore; break;
        case 154:
            id = blocks::minecraft::hopper;
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
            break;
        case 155:
            switch (data) {
            case 1:
                id = blocks::minecraft::chiseled_quartz_block;
                break;
            case 3:
                id = blocks::minecraft::quartz_pillar;
                props[u8"axis"] = u8"x";
                break;
            case 4:
                id = blocks::minecraft::quartz_pillar;
                props[u8"axis"] = u8"z";
                break;
            case 2:
                id = blocks::minecraft::quartz_pillar;
                props[u8"axis"] = u8"y";
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
            props[u8"facing"] = FacingA(data);
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
            case 1: id = blocks::minecraft::dark_oak_leaves; break;
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
            props[u8"axis"] = Axis(data >> 2);
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
            props[u8"rotation"] = String::ToString(data);
            break;
        case 177:
            id = blocks::minecraft::white_wall_banner;
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
            break;
        case 178:
            id = blocks::minecraft::daylight_detector;
            props[u8"inverted"] = u8"true";
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
            props[u8"type"] = u8"double";
            break;
        case 182:
            switch (data) {
            case 8:
                id = blocks::minecraft::red_sandstone_slab;
                props[u8"type"] = u8"top";
                break;
            case 0:
            default:
                id = blocks::minecraft::red_sandstone_slab;
                props[u8"type"] = u8"bottom";
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
            props[u8"facing"] = FacingA(data);
            break;
        case 199: id = blocks::minecraft::chorus_plant; break;
        case 200:
            id = blocks::minecraft::chorus_flower;
            props[u8"age"] = String::ToString(data);
            break;
        case 201: id = blocks::minecraft::purpur_block; break;
        case 202:
            id = blocks::minecraft::purpur_pillar;
            props[u8"axis"] = Axis(data >> 2);
            break;
        case 203:
            id = blocks::minecraft::purpur_stairs;
            Stairs(data, props);
            break;
        case 204:
            id = blocks::minecraft::purpur_slab;
            props[u8"type"] = u8"double";
            break;
        case 205:
            id = blocks::minecraft::purpur_slab;
            if (data == 8) {
                props[u8"type"] = u8"top";
            } else {
                props[u8"type"] = u8"bottom";
            }
            break;
        case 206: id = blocks::minecraft::end_stone_bricks; break;
        case 207:
            id = blocks::minecraft::beetroots;
            props[u8"age"] = String::ToString(data);
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
            props[u8"axis"] = Axis(data >> 2);
            break;
        case 217: id = blocks::minecraft::structure_void; break;
        case 218:
            id = blocks::minecraft::observer;
            props[u8"facing"] = FacingA(data);
            break;
        case 219:
            id = blocks::minecraft::white_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 220:
            id = blocks::minecraft::orange_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 221:
            id = blocks::minecraft::magenta_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 222:
            id = blocks::minecraft::light_blue_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 223:
            id = blocks::minecraft::yellow_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 224:
            id = blocks::minecraft::lime_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 225:
            id = blocks::minecraft::pink_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 226:
            id = blocks::minecraft::gray_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 227:
            id = blocks::minecraft::light_gray_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 228:
            id = blocks::minecraft::cyan_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 229:
            id = blocks::minecraft::shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 230:
            id = blocks::minecraft::blue_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 231:
            id = blocks::minecraft::brown_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 232:
            id = blocks::minecraft::green_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 233:
            id = blocks::minecraft::red_shulker_box;
            props[u8"facing"] = FacingA(data);
            break;
        case 234:
            id = blocks::minecraft::black_shulker_box;
            props[u8"facing"] = FacingA(data);
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
