#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_graphics.h"
#include "battle.h"
#include "utils.h"

char *Messages[] =
    {
        "A Heated Battle",
        "Next",
        "Player 1 Instance",
        "Player 2 Instance",
};

void start_battle(global_state *g)
{
    g->state = BATTLE;
    g->current_action = NULL;
    for (int i = 0; i < MAX_BAT_EN; i++)
    {
        g->cur_enemies[i].monster = &g->enemies[rand() % NUM_ENEMIES];
        g->cur_enemies[i].hp = g->cur_enemies[i].monster->max_hp;
    }
    g->n_enemies = rand() % MAX_BAT_EN + 1;
    slog("Num enemies = %d\n", g->n_enemies);
    for (int i = 0; i < g->n_allies; i++)
    {
        // g->allies[i].hp = g->allies[i].monster->max_hp;
    }
    g->press_time = SDL_GetTicks();
    // TODO
    strcpy(g->info_out, "Heated battle");
}

void battle_menu_output(struct nk_context *ctx, char *info_out)
{
    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
    if (nk_begin(ctx, "top 4", nk_rect(600, 500, 300, 80), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, info_out, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void battle_menu_enemy(struct nk_context *ctx, monst_inst *inst)
{
    char enemy_status[200];
    Character *c = inst->monster;

    snprintf(enemy_status, 200, "ENEMY Level %i | HP: %i/%i", c->level, inst->hp, c->max_hp);
    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
    if (nk_begin(ctx, "top 3", nk_rect(600, 50, 200, 80), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, c->name, NK_TEXT_LEFT);

        nk_label(ctx, enemy_status, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void battle_enemies(global_state *g)
{
    struct nk_context *ctx = g->ctx;
    char buf[200];

    monst_inst *e = g->cur_enemies;

    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
    if (nk_begin(ctx, "Enemies", nk_rect(900, 500, 200, 180), flags))
    {
        nk_layout_row_dynamic(ctx, 0, 1);

        for (int i = 0; i < g->n_enemies; i++)
        {
            Character *c = e[i].monster;
            snprintf(buf, 200, "%s:  %d/%d HP", c->name, e[i].hp, c->max_hp);

            nk_label(ctx, buf, NK_TEXT_LEFT);
        }
    }
    nk_end(ctx);
}

void battle_menu_attack(global_state *g, monst_inst *inst)
{
    struct nk_context *ctx = g->ctx;
    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;

    char ally_status[200];
    Character *c = inst->monster;
    if (nk_begin(ctx, "top 1", nk_rect(10, 500, 200, 200), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, c->name, NK_TEXT_LEFT);

        snprintf(ally_status, 200, "Level %i | HP: %i/%i", c->level, inst->hp, c->max_hp);
        nk_label(ctx, ally_status, NK_TEXT_LEFT);

        // if (!g->current_action)
        if (!g->turn)
        {
            for (int i = 0; i < c->n_attacks; i++)
            {
                if (nk_button_label(ctx, c->attacks[i].name)) // render all buttons, and on button press do if statement
                {
                    // {
                    g->current_action = &c->attacks[i];
                    g->press_time = SDL_GetTicks();
                }
            }
        }
    }
    nk_end(ctx);
}

void battle_menu_item(global_state *g)
{
    int flags = NK_WINDOW_BORDER;
    struct nk_context *ctx = g->ctx;
    Inventory *in = &g->inventory;
    if (nk_begin(ctx, "top 2", nk_rect(300, 500, 200, 200), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, "ITEMS", NK_TEXT_LEFT);
        if (!g->turn)
        {
            for (int i = 0; i < in->num_item; i++)
            {
                if (nk_button_label(ctx, in->item[i].name))
                {
                    g->current_action = &in->item[i]; // fix later  u bum
                    g->press_time = SDL_GetTicks();
                }
            }
        }
    }
    nk_end(ctx);
}

/// @brief true = lethal, false is not
/// @param target
/// @param act
/// @return
int battle_action_dmg(global_state *g, monst_inst *target)
{
    Action *act = g->current_action;
    int damage = rand_range(act->min_dam, act->max_dam);
    slog("%s DID THIS %i", act->name, damage);
    target->hp -= damage;
    slog("ENEMY HP%i", target->hp);

    Character *c = target->monster;

    snprintf(g->info_out, 1024, "%s just took %d damage", c->name, damage);

    if (target->hp <= 0)
    {
        slog("%s has died", c->name);
        return TRUE;
    }
    else
    {
        slog("Continue Battle");
        return FALSE;
    }
}

/// @brief true = end, false = contine
/// @param ctx
/// @param info_out
/// @return
int battle_battle(global_state *g)
{
    int n_enemies = g->n_enemies;
    int n_allies = g->n_allies;
    int cur_enemy = g->cur_enemy;
    int cur_ally = g->cur_ally;

    // also updates cur_enemy/ally
    monst_inst *enemy_inst = &g->cur_enemies[cur_enemy];
    monst_inst *player_inst = &g->allies[cur_ally];
    // cur_enemy = enemy_inst - enemies_inst;

    monst_inst *players[] = {enemy_inst, player_inst};

    Character *player = player_inst->monster;
    Character *enemy = enemy_inst->monster;
    int cur_time = SDL_GetTicks();
    int time = cur_time - g->press_time;
    int died;
    if (g->current_action)
    {
        // if you kill them
        if ((died = battle_action_dmg(g, players[g->turn])))
        {
            // for now just replace 1 ally slot
            if (g->turn == PLAYER_TURN)
            {
                // allies_inst[1] = *enemy_inst; // last one who died
                // g->n_allies++; // < MAX_ALLIES

                g->n_enemies--;
                // if they're all dead
                if (!g->n_enemies)
                {
                    battle_load_new_world(g);
                    strcpy(g->info_out, "YOU WON THE BATTLE, +1 Level and More HP");
                    player->level++;
                    return FALSE;
                }

                // erase defeated enemy
                memmove(&g->cur_enemies[cur_enemy], &g->cur_enemies[cur_enemy + 1], (n_enemies - cur_enemy) * sizeof(monst_inst));
            }
            else
            {
                g->n_allies--;
                if (!g->n_allies)
                    return FALSE;

                // TODO game ends if all allies die?  if not we need
                // a separate array
                memmove(&g->allies[cur_ally], &g->allies[cur_ally + 1], (n_allies - cur_ally) * sizeof(monst_inst));
            }
        }
        else
        {
            if (!died || g->turn == PLAYER)
            {
                g->cur_ally++;
                g->cur_ally %= n_allies;
            }
            if (!died || g->turn == ENEMY)
            {
                g->cur_enemy++;
                g->cur_enemy %= n_enemies;
            }
        }
        g->current_action = NULL;

        g->turn = !g->turn;

        slog(g->info_out);
        g->press_time = cur_time;
    }
    else if (time > TURN_DELAY)
    {
        // if player is target, choose enemy attack
        if (g->turn == ENEMY_TURN)
        {
            // TODO pick random attack
            g->current_action = &enemy->attacks[0];
        }
        else
        {
            snprintf(g->info_out, 1024, "%s pick a new attack", player->name);
            g->press_time = cur_time;
        }
    }
    gf2d_sprite_draw_image(player->sprite, vector2d(0, 0));
    gf2d_sprite_draw_image(enemy->sprite, vector2d(700, -150));
    battle_menu_attack(g, player_inst);
    battle_menu_enemy(g->ctx, enemy_inst);
    battle_menu_item(g);
    // battle_menu_output(g->ctx, g->info_out);

    battle_enemies(g);

    // TODO move out to main
    nk_sdl_render(NK_ANTI_ALIASING_ON);
    return TRUE;
}

Character battle_load_character(char *name)
{
    SJson *lj;
    Character charJson;
    char path[512];
    strcpy(path, "config/");
    strcat(path, name);
    // strcat(path, ".json");
    lj = sj_load(path);

    strcpy(charJson.sprite_path, sj_object_get_value_as_string(lj, "sprite_path"));
    printf("%s\n", charJson.sprite_path);

    charJson.sprite = gf2d_sprite_load_image(charJson.sprite_path);
    strcpy(charJson.name, sj_object_get_value_as_string(lj, "name"));
    sj_object_get_value_as_int(lj, "level", &charJson.level);
    sj_object_get_value_as_int(lj, "hp", &charJson.hp); // dont need to list for enemy, always gets initalized to max hp, only for player
    sj_object_get_value_as_int(lj, "max_hp", &charJson.max_hp);
    sj_object_get_value_as_int(lj, "n_attacks", &charJson.n_attacks);
    SJson *attackJson = sj_object_get_value(lj, "attacks");
    for (int i = 0; i < charJson.n_attacks; i++)
    {
        SJson *jattack = sj_array_get_nth(attackJson, i);

        strcpy(charJson.attacks[i].name, sj_object_get_value_as_string(jattack, "name"));
        sj_object_get_value_as_int(jattack, "min_dam", &charJson.attacks[i].min_dam);
        sj_object_get_value_as_int(jattack, "max_dam", &charJson.attacks[i].max_dam);
    }
    charJson.max_hp = charJson.max_hp * charJson.level;
    return charJson;
    // slog(charJson.attacks[6].name);
    // slog("%i",testJson.attacks[0].min_dam);
}

// Character battle_random_enemy(){
//     int upper = 2;
//     int lower = 1;
//     int num = (rand() %(upper - lower + 1)) + lower;
//     return num;
// }

// TODO still 1/2 broken, gotta fix
void battle_save_data_character(char *ipath, Character *chr_save)
{
    char path[512];
    strcpy(path, "config/");
    strcat(path, ipath);
    SJson *source = sj_load(path);
    SJson *charjson = sj_object_get_value(source, "character");
    SJson *dest = sj_copy(source);
    sj_object_delete_key(dest, "character");
    SJson *num;

    num = sj_new_int(234);
    sj_object_delete_key(charjson, "level");
    sj_object_insert(charjson, "level", num);
    sj_object_insert(dest, "character", charjson);

    sj_save(dest, "config/test.json");
}

void battle_load_new_world(global_state *g)
{
    char buf[1024];
    snprintf(buf, 1024, "config/levels/level%d.json", rand_range(0, 2));
    level_set_active_level(level_load(buf));

    snprintf(buf, 1024, "config/chests/chest%d.json", rand_range(0, 3));
    g->chests = level_load(buf);
}