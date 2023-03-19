#ifndef GLOBALS_H
#define GLOBALS_H

#include "nuklear.h"

#include "characters.h"

#define TRUE 1
#define FALSE 0

// Who is the target
#define PLAYER 0
#define ENEMY 1

#define TURN_DELAY 2000
#define NUM_ENEMIES 10
#define MAX_BAT_EN 4
#define MAX_ALLIES 1
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

    Character enemies[NUM_ENEMIES];

    int n_enemies;
    monst_inst cur_enemies[MAX_BAT_EN];

    int n_allies;
    monst_inst allies[1+MAX_ALLIES];

    Inventory inventory;
    //int n_items;
    //Action Inventory[INVENTORY_SZ]

    // a "level" of positions of chests
    Level* chests;

    Vector2D world_center;
} global_state;

#endif
