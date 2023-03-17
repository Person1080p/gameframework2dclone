#include <SDL.h>
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "level.h"
#include "camera.h"

#include "gme_entity.h"

#include "battle.h"


enum
{
    DELAY,
    ALWAYS,
    NEVER
};

int fullscreen_gui;

#define TRUE 1
#define FALSE 0

Inventory inventory =
    {
        2,
        {{"Heal", -30, -30}, {"Fire", 20, 30}}

};

Character Player =
    {
        NULL,
        "images/demon/4.png",
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
        NULL,
        "images/demon/2.png",
        "Pixie",
        1,
        120,
        120,
        7,
        {{"Tackle",
          10,
          15}}};

int battle_now = 0;

// State names rather than matching the string

int main(int argc, char *argv[])
{
    SJson *json,*lj;
    Character testJson;
    char path[512];
    char* name = "player";
    strcpy(path, "config/");
    strcat(path, name);
    strcat(path, ".json");
    json = sj_load(path);
    lj = sj_object_get_value(json,"character");

    testJson.sprite = NULL;
    strcpy(testJson.sprite_path, sj_object_get_value_as_string(lj,"sprite_path"));
    strcpy(testJson.name, sj_object_get_value_as_string(lj,"name"));
    sj_object_get_value_as_int(lj,"level", &testJson.level);
    sj_object_get_value_as_int(lj,"hp", &testJson.hp);
    sj_object_get_value_as_int(lj,"max_hp", &testJson.max_hp);
    sj_object_get_value_as_int(lj,"n_attacks", &testJson.n_attacks);
    SJson *attackJson = sj_object_get_value(lj,"attacks");
    for(int i=0; i<testJson.n_attacks; i++){
        SJson *jattack = sj_array_get_nth(attackJson,i);

        strcpy(testJson.attacks[i].name, sj_object_get_value_as_string(jattack,"name"));
        sj_object_get_value_as_int(jattack,"min_dam", &testJson.attacks[i].min_dam);
        sj_object_get_value_as_int(jattack,"max_dam", &testJson.attacks[i].max_dam);

    }

    slog(testJson.attacks[6].name);
    // slog("%i",testJson.attacks[0].min_dam);

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

    // TODO in a loop read in json (or whatever) and load all sprites
    // arrays of players (if you have more than one player) and enemys
    Player.sprite = gf2d_sprite_load_image(Player.sprite_path);
    Enemy.sprite = gf2d_sprite_load_image(Enemy.sprite_path);

    exent = gme_entity_new();
    exent->entity_sprite = gf2d_sprite_load_all(
        "images/space_bug.png",
        128,
        128,
        16,
        0);

    battle_now = TRUE;

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

        // SDL_asprintf(&info_out, "Char's %s did %d", current_action->name, (current_action->max_dam, current_action->min_dam));
        // SDL_asprintf(&info_out, "Char's %s did Damage", current_action->name);
        // slog(info_out);
        // slog("Player Turn: %i", turn_player);
        if (keys[SDL_SCANCODE_P])
        {
            battle_now = 1;
            Enemy.hp = Enemy.max_hp;
        }
        if (battle_now)
        {
            battle_now = battle_battle(ctx, &Player, &Enemy, &inventory);
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
            // battle_now = TRUE;

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

/*eol@eof*/
