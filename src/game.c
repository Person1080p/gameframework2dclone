#include <SDL.h>
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "level.h"
#include "camera.h"

#include "gme_entity.h"

enum
{
    DELAY,
    ALWAYS,
    NEVER
};

int fullscreen_gui;

/// @brief Used for Both Abilities and Damage
typedef struct Action
{
    char name[20];
    int min_dam;
    int max_dam;
} Action;

int turn_player;

Action *current_action;
#define TURN_DELAY 2000
int press_time;

#define TRUE 1
#define FALSE 0
typedef struct Character
{
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

Inventory inventory =
    {
        2,
        {{"Heal", -30, -30}, {"Fire", 20, 30}}

};

Character Player =
    {
        "Player_Char",
        1,
        120,
        120,
        7,
        {{"Tackle",
          10,
          15},
         {"Fire",
          130,
          330},
         {"Fire2",
          10,
          30},
         {"Fire3",
          10,
          30},
         {"Fire4",
          10,
          30},
         {"Fire5",
          10,
          30},
         {"Fire6",
          10,
          30}}};

Character Enemy =
    {
        "Pixie",
        1,
        120,
        120,
        7,
        {{"Tackle",
          10,
          15}}};

void menu_output(struct nk_context *ctx, char *out);

void menu_enemy(struct nk_context *ctx, Character *c);

void menu_attack(struct nk_context *ctx, Character *c);

void menu_item(struct nk_context *ctx, Inventory *i);

int action_rng(int max, int min);

int action_dmg(Character *enemyc, Action act);

int battle();

int battle_now = 0;

int main(int argc, char *argv[])
{
    char *out = "A Heated Battle";
    turn_player = 0; // now player turn
    /*variable declarations*/
    int done = 0;
    const Uint8 *keys;
    Sprite *background;
    Level *level;
    Entity *exent;
    // gme_entity_init(1024);
    gme_entity_manager_init(1024);

    /*mouse vars*/
    int mx, my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255, 153, 0, 150);

    /*program initializtion*/
    init_logger("gf2d.log", 0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "game",
        1200,
        720,
        1200,
        720,
        vector4d(0, 0, 0, 255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    // gme_entity_manager_init(1024);
    SDL_ShowCursor(SDL_DISABLE);

    struct nk_context *ctx = gf2d_nuklear_init();
    SDL_Event evt;

    /*demo setup*/
    background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16, 0);

    level = level_load("config/test.level");
    level_set_active_level(level);

    exent = gme_entity_new();
    exent->entity_sprite = gf2d_sprite_load_all(
        "images/space_bug.png",
        128,
        128,
        16,
        0);

    while (!done)
    {

        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
            {
                done = 1;
                break;
            }
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(ctx);

        SDL_PumpEvents(); // update SDL's internal event structures
        /*handle keyboard and mouse events here*/
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx, &my);
        mf += 0.1;
        if (mf >= 16.0)
            mf = 0;
        //

        //
        gf2d_graphics_clear_screen(); // clears drawing buffers
                                      // all drawing should happen betweem clear_screen and next_frame
        // backgrounds drawn first
        gf2d_sprite_draw_image(background, vector2d(0, 0));
        // contents next
        // UI elements last

        // SDL_asprintf(&out, "Char's %s did %d", current_action->name, (current_action->max_dam, current_action->min_dam));
        // SDL_asprintf(&out, "Char's %s did Damage", current_action->name);
        // slog(out);
        // slog("Player Turn: %i", turn_player);
        if (battle_now)
        {
            if (battle(ctx, out))
            {
                battle_now = FALSE;
            }
        }
        else
        {
            gme_entity_animate(exent, 0, 16.0);
            // exent->position = vector2d(mf*10+mf,mf*10+mf);
            float speed = 10;
            if (keys[SDL_SCANCODE_W])
                exent->position = vector2d(exent->position.x, exent->position.y - speed);
            if (keys[SDL_SCANCODE_S])
                exent->position = vector2d(exent->position.x, exent->position.y + speed);
            if (keys[SDL_SCANCODE_A])
                exent->position = vector2d(exent->position.x - speed, exent->position.y);
            if (keys[SDL_SCANCODE_D])
                exent->position = vector2d(exent->position.x + speed, exent->position.y);

            // if (exent->position.y < 0)
            //     exent->position.y = 0;
            // if (exent->position.x > 1080)
            //     exent->position.x = 1080;
            // if (exent->position.y > 600)
            //     exent->position.y = 600;
            // if (exent->position.x < 0)
            //     exent->position.x = 0;

            slog("Bug Pos: %f, %f", exent->position.x, exent->position.y);

            // start->position=vector2d(mx,my);
            // gme_entity_animate(exent, 1, 16.0);
            level_draw(level_get_active_level());
            gme_entity_draw_all();
            gme_entity_think(exent);
            // Vector2D mousep = vector2d(mx,my);
            camera_world_snap();
        }

        gf2d_sprite_draw(
            mouse,
            vector2d(mx, my),
            NULL,
            NULL,
            NULL,
            NULL,
            &mouseColor,
            (int)mf);

        gf2d_graphics_next_frame(); // render current draw frame and skip to the next frame

        // camera_center_at(mousep);
        // slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
        // slog("Mouse at %i , %i",mx,my);
        if (keys[SDL_SCANCODE_ESCAPE])
            done = 1; // exit condition
    }
    level_free(level);
    nk_sdl_shutdown();
    slog("---==== END ====---");
    return 0;
}

void menu_output(struct nk_context *ctx, char *out)
{
    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
    if (nk_begin(ctx, "top 4", nk_rect(600, 500, 200, 80), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, out, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void menu_enemy(struct nk_context *ctx, Character *c)
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

void menu_attack(struct nk_context *ctx, Character *c)
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
                    turn_player = 1; // no more player turn
                    slog("Player Turn: %i", turn_player);
                    current_action = &c->attacks[i];
                    press_time = SDL_GetTicks();
                }
            }
        }
    }
    nk_end(ctx);
}
void menu_item(struct nk_context *ctx, Inventory *in)
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

int action_rng(int max, int min)
{
    return rand() % (max - min) + min;
}
/// @brief true = lethal, false is not
/// @param target
/// @param act
/// @return
int action_dmg(Character *target, Action act)
{
    int damage = action_rng(act.max_dam, act.min_dam);
    slog("SPELL DID THIS %i", damage);
    target->hp -= damage;
    slog("ENEMY HP%i", target->hp);
    if (target->hp < 0)
    {
        slog("YOU WIN");
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
/// @param out
/// @return
int battle(struct nk_context *ctx, char *out)
{
    Sprite *playerSprite = gf2d_sprite_load_image("images/demon/4.png");
    Sprite *enemySprite = gf2d_sprite_load_image("images/demon/2.png");
    int cur_time = SDL_GetTicks();
    if (current_action)
    {
        int time = cur_time - press_time;
        // slog("%i",time);

        if (time > TURN_DELAY * 3)
        {
            out = "Next";
            current_action = NULL;
        }
        else if (time > TURN_DELAY * 2)
        {
            current_action = &Enemy.attacks[0];
            action_dmg(&Player, *current_action);
            out = "Player 2 Instance";
            slog(out);
            slog("Player Turn: %i", turn_player);
        }
        else if (time > TURN_DELAY)
        {
            if (action_dmg(&Enemy, *current_action))
            {
                return FALSE;
            }
            // current_action = NULL;
            out = "Player 1 Instance";
            slog(out);
            // slog("Player Turn: %i", turn_player);
            // press_time = SDL_GetTicks();
        }
    }
    // if(cur_time - attack_timer > ATTACK_DELAY){
    //     out ="Battle Continues";
    // }
    gf2d_sprite_draw_image(playerSprite, vector2d(0, 0));
    gf2d_sprite_draw_image(enemySprite, vector2d(700, -150));
    menu_attack(ctx, &Player);
    menu_enemy(ctx, &Enemy);
    menu_item(ctx, &inventory);
    menu_output(ctx, out);
    nk_sdl_render(NK_ANTI_ALIASING_ON);
    return FALSE;
}
/*eol@eof*/
