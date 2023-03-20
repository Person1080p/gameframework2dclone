#ifndef BATTLE_H
#define BATTLE_H

#include "gf2d_sprite.h"
#include "globals.h"

// TODO put Messages an enum in globals?
enum
{
    HEATED,
    NEXT,
    P1_INST,
    P2_INST,
    NUM_MESSAGES
};

extern char* Messages[];

void start_battle(global_state* g);

void battle_menu_output(struct nk_context *ctx, char *info_out);

void battle_menu_enemy(struct nk_context *ctx, monst_inst* inst);

void battle_menu_attack(global_state* g, monst_inst* inst);

void battle_menu_item(global_state* g);

int battle_action_rng(int max, int min);

int battle_action_dmg(global_state* g, monst_inst *target);

int battle_battle(global_state* g);

void battle_enemies(global_state* g);

/// @brief loads a character (enemy or ally) from a json file
/// @param name path to character json relative to config folder
/// @return loaded character object
Character battle_load_character(char *name);

// Character battle_random_enemy();

void battle_save_data_character(char *ipath, Character *chr_save);

void battle_load_new_world(global_state *g);

#endif
