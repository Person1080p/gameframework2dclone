#ifndef CHARACTERS_H
#define CHARACTERS_H

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

// rename char instance?
typedef struct monst_inst
{
    Character* monster;
    int hp;
} monst_inst;



typedef struct Inventory
{
    int num_item;
    Action item[20];
    // add item amounts
} Inventory;

#endif
