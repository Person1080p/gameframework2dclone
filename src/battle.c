#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_graphics.h"
#include "battle.h"

#define TRUE 1
#define FALSE 0

#define TURN_DELAY 2000

Action *current_action;
int press_time;
char info_out[1024];

enum
{
    HEATED,
    NEXT,
    P1_INST,
    P2_INST,
    NUM_MESSAGES
};

char *Messages[] =
    {
        "A Heated Battle",
        "Next",
        "Player 1 Instance",
        "Player 2 Instance",
};

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

void battle_menu_enemy(struct nk_context *ctx, Character *c)
{
    char *enemy_status;
    SDL_asprintf(&enemy_status, "ENEMY Level %i | HP: %i/%i", c->level, c->hp, c->max_hp);
    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
    if (nk_begin(ctx, "top 3", nk_rect(600, 50, 200, 80), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, c->name, NK_TEXT_LEFT);

        nk_label(ctx, enemy_status, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void battle_menu_attack(struct nk_context *ctx, Character *c)
{
    int flags = NK_WINDOW_BORDER;
    if (nk_begin(ctx, "top 1", nk_rect(10, 500, 200, 200), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, c->name, NK_TEXT_LEFT);

        char *ally_status;
        SDL_asprintf(&ally_status, "Level %i | HP: %i/%i", c->level, c->hp, c->max_hp);
        nk_label(ctx, ally_status, NK_TEXT_LEFT);

        if (!current_action)
        {
            for (int i = 0; i < c->n_attacks; i++)
            {
                if (nk_button_label(ctx, c->attacks[i].name)) // render all buttons, and on button press do if statement
                {
                    current_action = &c->attacks[i];
                    press_time = SDL_GetTicks();
                }
            }
        }
    }
    nk_end(ctx);
}
void battle_menu_item(struct nk_context *ctx, Inventory *in)
{
    int flags = NK_WINDOW_BORDER;
    if (nk_begin(ctx, "top 2", nk_rect(300, 500, 200, 200), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, "ITEMS", NK_TEXT_LEFT);

        for (int i = 0; i < in->num_item; i++)
        {
            if (nk_button_label(ctx, in->item[i].name))
            {
                current_action = &in->item[i]; // fix later  u bum
                press_time = SDL_GetTicks();
            }
        }
    }
    nk_end(ctx);
}

int battle_action_rng(int min, int max)
{
    return rand() % (max - min) + min;
}
/// @brief true = lethal, false is not
/// @param target
/// @param act
/// @return
int battle_action_dmg(Character *target, Action act)
{
    int damage = battle_action_rng(act.min_dam, act.max_dam);
    slog("%s DID THIS %i", act.name, damage);
    target->hp -= damage;
    slog("ENEMY HP%i", target->hp);

    snprintf(info_out, 1024, "%s just took %d damage", target->name, damage);

    if (target->hp <= 0)
    {
        slog("%s has died", target->name);
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
int battle_battle(struct nk_context *ctx, Character *player, Character *enemy, Inventory *inventory)
{
    static int whois_target = 0;
    // strcpy(info_out, Messages[HEATED]);
    // player goes first, meaning target is 0 enemy
    Character *players[] = {enemy, player};

    int cur_time = SDL_GetTicks();
    int time = cur_time - press_time;
    if (current_action)
    {
        // if you kill them
        if (battle_action_dmg(players[whois_target], *current_action))
        {
            whois_target = 0;
            current_action = NULL;
            return FALSE; // return false to end battle
        }

        whois_target = !whois_target;
        current_action = NULL;

        slog(info_out);
        press_time = cur_time;
    }
    else if (time > TURN_DELAY)
    {
        // if player is target, choose enemy attack
        if (whois_target)
        {
            current_action = &enemy->attacks[0]; // chose randomly
        }
        else
        {
            snprintf(info_out, 1024, "%s pick a new attack", player->name);
        }
    }
    gf2d_sprite_draw_image(player->sprite, vector2d(0, 0));
    gf2d_sprite_draw_image(enemy->sprite, vector2d(700, -150));
    battle_menu_attack(ctx, player);
    battle_menu_enemy(ctx, enemy);
    battle_menu_item(ctx, inventory);
    battle_menu_output(ctx, info_out);
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

    charJson.sprite = NULL;
    strcpy(charJson.sprite_path, sj_object_get_value_as_string(lj, "sprite_path"));
    strcpy(charJson.name, sj_object_get_value_as_string(lj, "name"));
    sj_object_get_value_as_int(lj, "level", &charJson.level);
    sj_object_get_value_as_int(lj, "hp", &charJson.hp);
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
    return charJson;
    // slog(charJson.attacks[6].name);
    // slog("%i",testJson.attacks[0].min_dam);
}

Character battle_random_enemy(){
    int upper = 2;
    int lower = 1;
    int num = (rand() %(upper - lower + 1)) + lower;
}


//TODO still 1/2 broken, gotta fix
void battle_save_data_character(char *ipath, Character *chr_save)
{
    char path[512];
    strcpy(path, "config/");
    strcat(path, ipath);
    SJson* source = sj_load(path);
    SJson* charjson = sj_object_get_value(source, "character");
    SJson* dest = sj_copy(source);
    sj_object_delete_key(dest,"character");
    SJson* num;

    num = sj_new_int(234);
    sj_object_delete_key(charjson,"level");
    sj_object_insert(charjson,"level", num);
    sj_object_insert(dest,"character", charjson);

    sj_save(dest, "config/test.json");
}