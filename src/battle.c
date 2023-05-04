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
        g->cur_enemies[i].monster = &g->monsters[rand() % NUM_MONSTERS];
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

void battle_menu_character_status(struct nk_context *ctx, monst_inst *inst, int x, int y, char *title)
{
    char enemy_status[200];
    Character *c = inst->monster;
    char name_line[200];

    snprintf(name_line, 200, "%s STR:%s WK:%s", c->name, get_type_str(c->strong), get_type_str(c->weak));

    snprintf(enemy_status, 200, "Level %i | HP: %i/%i", c->level, inst->hp, c->max_hp);
    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
    if (nk_begin(ctx, title, nk_rect(x, y, 200, 80), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, name_line, NK_TEXT_LEFT);

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

    Character *c = inst->monster;
    char name_line[200];
    snprintf(name_line, 200, "%s STR:%s WK:%s", c->name, get_type_str(c->strong), get_type_str(c->weak));

    char ally_status[200];
    if (nk_begin(ctx, "top 1", nk_rect(10, 400, 200, 300), flags))
    {
        nk_layout_row_dynamic(ctx, 0, 1);

        nk_label(ctx, name_line, NK_TEXT_LEFT);

        snprintf(ally_status, 200, "Level %i | HP: %i/%i", c->level, inst->hp, c->max_hp);
        nk_label(ctx, ally_status, NK_TEXT_LEFT);

        // if (!g->current_action)
        if (!g->turn)
        {
            for (int i = 0; i < c->n_attacks; i++)
            {
                if (nk_button_label(ctx, c->attacks[i].name)) // render all buttons, and on button press do if statement
                {
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
    char* modifier = "";
    // handle character/attack categories/affinities
    if (act->type != NONE) {
        if (target->monster->strong == act->type) {
            slog("Hit Resistance");
            modifier = "[RESIST]";
            damage /= 2;
        } else if (target->monster->weak == act->type) {
            slog("Hit Weakness");
            modifier = "[WEAK]";
            damage *= 2;
        }
    }

    slog("%s DID THIS %i %s", act->name, damage,modifier);
    target->hp -= damage;
    slog("ENEMY HP%i", target->hp);

    Character *c = target->monster;

    snprintf(g->info_out, 1024, "%s just took %d damage %s", c->name, damage,modifier);

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

    // TODO if not player turn, pick random target for enemy, not cur_ally
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
                    battle_save_data_character("player_data/player.json", player);
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

                // TODO game ends if all allies die
                memmove(&g->allies[cur_ally], &g->allies[cur_ally + 1], (n_allies - cur_ally) * sizeof(monst_inst));
            }
        }

        if (g->turn == PLAYER_TURN)
        {
            g->cur_ally++;
            if (g->cur_ally == n_allies)
            {
                g->turn = !g->turn;
            }
            g->cur_ally %= n_allies;
        }
        else
        { // if (g->turn == ENEMY_TURN)
            g->cur_enemy++;
            if (g->cur_enemy == n_enemies)
            {
                g->turn = !g->turn;
            }
            g->cur_enemy %= n_enemies;
        }
        g->current_action = NULL;

        slog(g->info_out);
        g->press_time = cur_time;
    }
    else if (time > TURN_DELAY)
    {
        // if player is target, choose enemy attack
        if (g->turn == ENEMY_TURN)
        {
            g->current_action = &enemy->attacks[rand_range(0, enemy->n_attacks)];
        }
        else
        {
            snprintf(g->info_out, 1024, "%s pick a new attack", player->name);
            g->press_time = cur_time;
        }
    }
    char winbuf[1024];
    for (int i = 0; i < g->n_enemies; i++)
    {
        snprintf(winbuf, 1024, "enemytitle%i", i);
        battle_menu_character_status(g->ctx, &g->cur_enemies[i], (400 + 200 * i), 200, winbuf);
        gf2d_sprite_draw_image(g->cur_enemies[i].monster->sprite, vector2d((300 + 200 * i), -150));
    }
    for (int i = 0; i < g->n_allies; i++)
    {
        snprintf(winbuf, 1024, "allytitle%i", i);
        battle_menu_character_status(g->ctx, &g->allies[i], (400 + 200 * i), 400, winbuf);
        gf2d_sprite_draw_image(g->allies[i].monster->sprite, vector2d((300 + 200 * i), 0));
        // gf2d_sprite_draw_image(player->sprite, vector2d(0, 0));
    }
    // gf2d_sprite_draw_image(enemy->sprite, vector2d(700, -150));
    battle_menu_attack(g, player_inst);

    battle_menu_item(g);
    // battle_menu_output(g->ctx, g->info_out);

    battle_enemies(g);

    nk_sdl_render(NK_ANTI_ALIASING_ON);
    return TRUE;
}

int get_type(const char* s)
{
    if (!s) {
        return NONE;
    }
    const char* type[] = {
        "NONE",
        "FIRE",
        "ICE",
        "AIR",
        "ELECTRIC",
        "EARTH",
        "WATER",
        "LIGHT",
        "DARK"
    };
    for (int i=0; i<NUM_TYPES; i++) {
        if (!strcmp(type[i], s)) {
            return i;
        }
    }

    return NONE;
}

char* get_type_str(int type)
{
    const char* type_strs[] = {
        "NONE",
        "FIRE",
        "ICE",
        "AIR",
        "ELECTRIC",
        "EARTH",
        "WATER",
        "LIGHT",
        "DARK"
    };

    if (type < 0 || type >= NUM_TYPES) {
        type = 0;
    }

    return type_strs[type];
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

    charJson.strong = get_type(sj_object_get_value_as_string(lj, "strong"));
    charJson.weak = get_type(sj_object_get_value_as_string(lj, "weak"));

    SJson *attackJson = sj_object_get_value(lj, "attacks");
    for (int i = 0; i < charJson.n_attacks; i++)
    {
        SJson *jattack = sj_array_get_nth(attackJson, i);

        strcpy(charJson.attacks[i].name, sj_object_get_value_as_string(jattack, "name"));
        sj_object_get_value_as_int(jattack, "min_dam", &charJson.attacks[i].min_dam);
        sj_object_get_value_as_int(jattack, "max_dam", &charJson.attacks[i].max_dam);

        charJson.attacks[i].type = get_type(sj_object_get_value_as_string(jattack, "type"));
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

void battle_save_data_character(char *ipath, Character *chr_save)
{
    char path[512];
    strcpy(path, "config/");
    strcat(path, ipath);
    SJson *source = sj_load(path);
    SJson *dest = sj_copy(source);
    SJson *char_level;

    char_level = sj_new_int(chr_save->level + 1);
    sj_object_delete_key(dest, "level");
    sj_object_insert(dest, "level", char_level);

    sj_save(dest, path);
}

void battle_load_new_world(global_state *g)
{
    // if(level_get_active_level())
    // {
    // level_free(level_get_active_level());
    // level_free(g->chests);
    // level_free(g->encounters);
    // level_free(g->npc);
    // }
    Vector2D pos = vector2d(750, 750);
    g->ent->position = pos;
    char buf[1024];
    snprintf(buf, 1024, "config/levels/level%d.json", rand_range(0, 2));
    level_set_active_level(level_load(buf));

    snprintf(buf, 1024, "config/chests/chest%d.json", rand_range(0, 3));
    g->chests = level_load(buf);

    snprintf(buf, 1024, "config/enco/enco%d.json", rand_range(0, 3));
    g->encounters = level_load(buf);

    snprintf(buf, 1024, "config/npc/npc%d.json", rand_range(0, 3));
    g->npc = level_load(buf);

    snprintf(buf, 1024, "config/door/door%d.json", rand_range(0, 3));
    g->door = level_load(buf);

    snprintf(buf, 1024, "config/lava/lava%d.json", rand_range(0, 3));
    g->lava = level_load(buf);
}
