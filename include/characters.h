#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "gf2d_sprite.h"

enum {
	NONE,
	FIRE,
	ICE,
	AIR,
	ELECTRIC,
	EARTH,
	WATER,
	LIGHT,
	DARK,
	NUM_TYPES
};

/// @brief Used for Both Abilities and Damage
typedef struct Action
{
    char name[20];
    int min_dam;
    int max_dam;
    int type;
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
    int weak;
    int strong;
} Character;

// rename char instance?
typedef struct monst_inst
{
    Character *monster;
    int hp;
} monst_inst;

typedef struct Inventory
{
    int num_items;
    int item_counts[20];
    Action items[20];
} Inventory;

#endif
