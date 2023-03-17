#ifndef BATTLE_H
#define BATTLE_H

#include <SDL.h>
#include "gf2d_sprite.h"

/// @brief Used for Both Abilities and Damage
typedef struct Action
{
    char name[20];
    int min_dam;
    int max_dam;
} Action;

typedef struct Character
{
    Sprite *sprite;
    char sprite_path[1024];
    char name[50];
    int level;
    int hp;
    int max_hp;
    int n_attacks;
    Action attacks[7];
} Character;

typedef struct Inventory
{
    int num_item;
    Action item[20];
    // add item amounts
} Inventory;

void battle_menu_output(struct nk_context *ctx, char *info_out);

void battle_menu_enemy(struct nk_context *ctx, Character *c);

void battle_menu_attack(struct nk_context *ctx, Character *c);

void battle_menu_item(struct nk_context *ctx, Inventory *i);

int battle_action_rng(int max, int min);

int battle_action_dmg(Character *enemyc, Action act);

int battle_battle(struct nk_context *ctx, Character *player, Character *enemy, Inventory *inventory);

Character battle_load_character(char *name);

void testwrite();

#endif