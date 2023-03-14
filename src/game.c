#include <SDL.h>
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

enum
{
    DELAY,
    ALWAYS,
    NEVER
};

int fullscreen_gui;

typedef struct Attack
{
    char name[20];
    int min_dam;
    int max_dam;
} Attack;

Attack *current_attack;
#define ATTACK_DELAY 2000
int attack_timer;

typedef struct Character
{
    char name[50];
    int level;
    int hp;
    int max_hp;
    int n_attacks;
    Attack attacks[5];
} Character;

Character Pikachu =
    {
        "Player",
        1,
        120,
        120,
        2,
        {{"Tackle",
          10,
          15},
         {"Fire",
          10,
          30}}};

void menu_attack(struct nk_context *ctx, Character *c);

int main(int argc, char *argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *background;

    // Entity *exent;

    /*mouse vars*/
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,153,0,150);



    /*program initializtion*/
    init_logger("gf2d.log",0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "game",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    // gme_entity_manager_init(1024);
    SDL_ShowCursor(SDL_DISABLE);

    struct nk_context *ctx = gf2d_nuklear_init();
    SDL_Event evt;

    /*demo setup*/
    background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);

    Sprite *enemy = gf2d_sprite_load_image("images/demon/2.png");
    Sprite *ally = gf2d_sprite_load_image("images/demon/4.png");
    // exent = gme_entity_new();
    // exent->entity_sprite = gf2d_sprite_load_all(
    //         "images/space_bug.png",
    //         128,
    //         128,
    //         16,
    //         0);
    
    // Entity *start = gme_entity_new();
    // start->entity_sprite = gf2d_sprite_load_all(
    //         "menu/start.png",
    //         32,
    //         32,
    //         1,
    //         0);
    // start->entity_sprite.scale
    /*main game loop*/
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

        SDL_PumpEvents();   // update SDL's internal event structures
        /*handle keyboard and mouse events here*/
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        //


        //
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(background,vector2d(0,0));
            //contents next
            //UI elements last

        gf2d_sprite_draw_image(enemy, vector2d(700, -150));
        gf2d_sprite_draw_image(ally, vector2d(0, 0));
        // Vector2D scale = vector2d(0,60);
        // gf2d_sprite_draw(pokemon,vector2d(50,50),&scale,NULL,NULL,NULL,NULL,0);

        int cur_time = SDL_GetTicks();
        if (current_attack && cur_time - attack_timer > ATTACK_DELAY)
        {
            // do attack
            slog("He did %d\n", rand() % (current_attack->max_dam - current_attack->min_dam) + current_attack->min_dam);
            current_attack = NULL;
            attack_timer = 0;
        }

        menu_attack(ctx, &Pikachu);
        // calculator(ctx);

        nk_sdl_render(NK_ANTI_ALIASING_ON);

        // UI elements last
        gf2d_sprite_draw(
            mouse,
            vector2d(mx,my),
            NULL,
            NULL,
            NULL,
            NULL,
            &mouseColor,
            (int)mf);

        gf2d_graphics_next_frame(); // render current draw frame and skip to the next frame

        if (keys[SDL_SCANCODE_ESCAPE])
            done = 1; // exit condition
                      // slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }

    nk_sdl_shutdown();
    slog("---==== END ====---");
    return 0;
}

void menu_attack(struct nk_context *ctx, Character *c)
{
    int flags = NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR;
    if (nk_begin(ctx, "top 1", nk_rect(10, 10, 200, 200), flags))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, c->name, NK_TEXT_LEFT);

        char *ally_status;
        SDL_asprintf(&ally_status, "Level %i | HP: %i/%i", c->level, c->hp, c->max_hp);
        nk_label(ctx, ally_status, NK_TEXT_LEFT);

        if (!current_attack)
        {
            for (int i = 0; i < c->n_attacks; i++)
            {
                if (nk_button_label(ctx, c->attacks[i].name))
                {
                    current_attack = &c->attacks[i];
                    attack_timer = SDL_GetTicks();
                }
            }
        }

        /*
        if (nk_option_label(ctx, "Delay", (fullscreen_gui == DELAY))) fullscreen_gui = DELAY;
        if (nk_option_label(ctx, "Always", (fullscreen_gui == ALWAYS))) fullscreen_gui = ALWAYS;
        if (nk_option_label(ctx, "Never", (fullscreen_gui == NEVER))) fullscreen_gui = NEVER;
        */
    }
    nk_end(ctx);
}

// static void
// calculator(struct nk_context *ctx)
// {
//     if (nk_begin(ctx, "Calculator", nk_rect(10, 10, 180, 250),
//                  NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_MOVABLE))
//     {
//         static int set = 0, prev = 0, op = 0;
//         static const char numbers[] = "789456123";
//         static const char ops[] = "+-*/";
//         static double a = 0, b = 0;
//         static double *current = &a;

//         size_t i = 0;
//         int solve = 0;
//         {
//             int len;
//             char buffer[256];
//             nk_layout_row_dynamic(ctx, 35, 1);
//             len = snprintf(buffer, 256, "%.2f", *current);
//             nk_edit_string(ctx, NK_EDIT_SIMPLE, buffer, &len, 255, nk_filter_float);
//             buffer[len] = 0;
//             *current = atof(buffer);
//         }

//         nk_layout_row_dynamic(ctx, 35, 4);
//         for (i = 0; i < 16; ++i)
//         {
//             if (i >= 12 && i < 15)
//             {
//                 if (i > 12)
//                     continue;
//                 if (nk_button_label(ctx, "C"))
//                 {
//                     a = b = op = 0;
//                     current = &a;
//                     set = 0;
//                 }
//                 if (nk_button_label(ctx, "0"))
//                 {
//                     *current = *current * 10.0f;
//                     set = 0;
//                 }
//                 if (nk_button_label(ctx, "="))
//                 {
//                     solve = 1;
//                     prev = op;
//                     op = 0;
//                 }
//             }
//             else if (((i + 1) % 4))
//             {
//                 if (nk_button_text(ctx, &numbers[(i / 4) * 3 + i % 4], 1))
//                 {
//                     *current = *current * 10.0f + numbers[(i / 4) * 3 + i % 4] - '0';
//                     set = 0;
//                 }
//             }
//             else if (nk_button_text(ctx, &ops[i / 4], 1))
//             {
//                 if (!set)
//                 {
//                     if (current != &b)
//                     {
//                         current = &b;
//                     }
//                     else
//                     {
//                         prev = op;
//                         solve = 1;
//                     }
//                 }
//                 op = ops[i / 4];
//                 set = 1;
//             }
//         }
//         if (solve)
//         {
//             if (prev == '+')
//                 a = a + b;
//             if (prev == '-')
//                 a = a - b;
//             if (prev == '*')
//                 a = a * b;
//             if (prev == '/')
//                 a = a / b;
//             current = &a;
//             if (set)
//                 current = &b;
//             b = 0;
//             set = 0;
//         }
//     }
//     nk_end(ctx);
// }

/*eol@eof*/
