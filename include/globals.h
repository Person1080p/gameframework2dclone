#ifndef GLOBALS_H
#define GLOBALS_H

#include "nuklear.h"
#include "entity.h"
#include "characters.h"

#define TRUE 1
#define FALSE 0

#define TURN_DELAY 2000
#define NUM_MONSTERS 10
#define MAX_BAT_EN 4
#define MAX_ALLIES 3
#define IVENTORY_SZ 20

enum { MAP, BATTLE };
enum { PLAYER_TURN, ENEMY_TURN };

#include "level.h"

typedef struct global_state
{
    struct nk_context *ctx;
    int fullscreen_gui;

    int state;
    int turn;
    int cur_ally, cur_enemy;

    Action *current_action;
    int press_time;
    char info_out[1024];

    Character monsters[NUM_MONSTERS];

    int n_enemies;
    monst_inst cur_enemies[MAX_BAT_EN];

    int n_allies;
    monst_inst allies[MAX_ALLIES];

    Inventory inventory;
    //int n_items;
    //Action Inventory[INVENTORY_SZ]

    // a "level" of positions of chests
    Level* chests;
    Level* encounters;
    Level* npc;
    Level* door;
    Level* lava;

    Entity *ent;

    Vector2D world_center;
} global_state;

#endif
