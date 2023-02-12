#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"

#include "gme_entity.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *background;

    Entity *exent;

    /*mouse vars*/
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,153,0,150);

    
    /*program initializtion*/
    init_logger("gf2d.log",0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    gme_entity_manager_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);

    exent = gme_entity_new();
    exent->entity_sprite = gf2d_sprite_load_all(
            "images/space_bug.png",
            128,
            128,
            16,
            0);
    
    Entity *start = gme_entity_new();
    start->entity_sprite = gf2d_sprite_load_all(
            "menu/start.png",
            32,
            32,
            1,
            0);
    // start->entity_sprite.scale
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        /*handle keyboard and mouse events here*/
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        


        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(background,vector2d(0,0));
            //contents next
            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);
            
            gme_entity_animate(exent, 0, 16.0);
            // exent->position = vector2d(mf*10+mf,mf*10+mf);
            float speed = 10;
            if (keys[SDL_SCANCODE_W]) exent->position = vector2d(exent->position.x,exent->position.y-speed);
            if (keys[SDL_SCANCODE_S]) exent->position = vector2d(exent->position.x,exent->position.y+speed);
            if (keys[SDL_SCANCODE_A]) exent->position = vector2d(exent->position.x-speed,exent->position.y);
            if (keys[SDL_SCANCODE_D]) exent->position = vector2d(exent->position.x+speed,exent->position.y);


            if(exent->position.y<0) exent->position.y=0;
            if(exent->position.x>1080) exent->position.x=1080;
            if(exent->position.y>600) exent->position.y=600;
            if(exent->position.x<0) exent->position.x=0;

            slog("Bug Pos: %f, %f",exent->position.x,exent->position.y);



            // start->position=vector2d(mx,my);
            // gme_entity_animate(exent, 1, 16.0);
            gme_entity_draw_all();
        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame
        //HANDLE KEYBOARD INPUT
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        // slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
        // slog("Mouse Pos: %i, %i",mx,my);
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
