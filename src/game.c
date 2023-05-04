#include <SDL.h>
#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "level.h"
#include "camera.h"

#include "entity.h"
#include "space_bug.h"

#include "battle.h"

#include "globals.h"

#include "utils.h"

enum
{
    DELAY,
    ALWAYS,
    NEVER
};

global_state *g;

void pause_menu(global_state *g);

int main(int argc, char *argv[])
{
    global_state mystate;
    g = &mystate;
    g->state = MENU;
    g->old_state = MAP;

    /*variable declarations*/
    const Uint8 *keys;

    Sprite *background;
    // gme_entity_init(1024);
    entity_manager_init(1024);

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

    g->ctx = gf2d_nuklear_init();
    SDL_Event e;
    int sc = e.key.keysym.scancode;

    /*demo setup*/
    background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16, 0);
    g->ent = space_bug_new(vector2d(750, 750));

    strcpy(g->info_out, " ");
    battle_load_new_world(g);

    Character Player = battle_load_character("player_data/player.json");
    monst_inst player_i = {&Player, Player.hp};
    g->allies[0] = player_i;
    g->n_allies = 1;

    char pathbuf[1024];
    for (int i = 0; i < NUM_MONSTERS; i++)
    {
        snprintf(pathbuf, 1024, "default_enemy/enemy%d.json", i);
        g->monsters[i] = battle_load_character(pathbuf);
    }
    g->world_center = vector2d(750, 750);
    /*
    g->n_items = 2;
    g->Inventory[0] = {"Heal", -30, -30};
    g->Inventory[1] = {"Fire", 20, 30};
    */
    g->inventory = (Inventory){2, {{"Heal", -30, -30}, {"Fire", 20, 30}}};

    // battle_save_data_character("player_data/player.json", &Player);

    slog(Player.attacks[6].name);

    // exent = entity_new();
    // exent->sprite = gf2d_sprite_load_all(
    //     "images/space_bug.png",
    //     128,
    //     128,
    //     16,
    //     0);

    while (g->state != EXIT)
    {

        nk_input_begin(g->ctx);
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                g->state = EXIT;
                break;
            }
            if (e.type == SDL_KEYUP)
            {
                sc = e.key.keysym.scancode;
                switch (sc)
                {
                case SDL_SCANCODE_ESCAPE:
                    if (g->state == MAP || g->state == BATTLE)
                    {
                        g->old_state = g->state;
                        g->state = MENU;
                    }
                    else if (g->state == MENU)
                    {
                        g->state = EXIT;
                    }
                    break;
                case SDL_SCANCODE_O:
                    battle_load_new_world(g);
                    break;
                }
            }
            nk_sdl_handle_event(&e);
        }
        nk_input_end(g->ctx);

        // SDL_PumpEvents(); // update SDL's internal event structures

        /*handle keyboard and mouse events here*/
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx, &my);
        mf += 0.1;
        if (mf >= 16.0)
            mf = 0;
        //
        entity_think_all();
        entity_update_all();
        camera_world_snap();
        //
        gf2d_graphics_clear_screen(); // clears drawing buffers
                                      // all drawing should happen betweem clear_screen and next_frame
        // backgrounds drawn first
        gf2d_sprite_draw_image(background, vector2d(0, 0));
        // contents next
        // UI elements last
        if (g->state == MAP && keys[SDL_SCANCODE_P])
        {
            start_battle(g);
        }

        if (g->state == BATTLE)
        {
            g->ent->position = g->world_center;
            if (!battle_battle(g))
            {
                g->state = MAP;
                // either exit or reset player/allies health
                if (!g->n_allies)
                {
                    return 0;
                }
            }
        }
        else if (g->state == MAP)
        {
            level_draw(g->chests);
            level_draw(g->encounters);
            level_draw(g->npc);
            level_draw(g->door);
            level_draw(g->lava);
            level_draw(level_get_active_level());
            entity_draw_all();
            camera_world_snap();

            if (keys[SDL_SCANCODE_P])
            {
                start_battle(g);
            }
        }
        else if (g->state == MENU)
        {
            pause_menu(g);
        }

        battle_menu_output(g->ctx, g->info_out);
        nk_sdl_render(NK_ANTI_ALIASING_ON);
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
        // slog("Char at %f , %f",ent->position.x,ent->position.y);
    }

    // TODO create cleanup function freeing all game state stuff
    level_free(level_get_active_level());
    level_free(g->chests);
    level_free(g->encounters);
    level_free(g->npc);
    level_free(g->door);
    level_free(g->lava);
    entity_free(g->ent);

    nk_sdl_shutdown();
    slog("---==== END ====---");
    return 0;
}

void pause_menu(global_state *g)
{
    struct nk_context *ctx = g->ctx;
    int flags = NK_WINDOW_BORDER;
    int alignment = NK_TEXT_CENTERED;

    if (nk_begin(ctx, "Main Menu", nk_rect(10, 500, 200, 200), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, "Main Menu", alignment);

        // Start/Resume
        if (nk_button_label(ctx, "Resume"))
        {
            g->state = g->old_state;
        }

        /*
        // settings/controls
        if (nk_button_label(ctx, ))
        {

        }
        */

        if (nk_button_label(ctx, "Exit"))
        {
            g->state = EXIT;
        }
    }
    nk_end(ctx);
}

/*eol@eof*/
