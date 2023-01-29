#include "gf2d_sprite.c"
#include "gf2d_sprite.h"
#include "gme_entity.h"
typedef struct{
    Uint32 max_entity;
    Entity * entity_list;
}EntityManager;

static EntityManager entity_manager;

void gme_entity_close(){
    // gme_entity_clear_all();

    if (entity_manager.entity_list != NULL){
        free(entity_manager.entity_list);
    }

    entity_manager.entity_list = NULL;
    entity_manager.max_entity = 0;
    slog("entity system closed");
}
void gme_entity_init(Uint32 max){//PROBABLY WRONG, CHECK l8R
    if (!max){
        slog("cannot intialize an entity manager for Zero entities!");
        return;
    }

    entity_manager.max_entity = max;
    entity_manager.entity_list = (Entity *)malloc(sizeof(Entity)*max);
    memset (entity_manager.entity_list,0,sizeof(Entity)*max);

    if (!(IMG_Init( IMG_INIT_PNG) & IMG_INIT_PNG)){
        slog("failed to init image: %s",SDL_GetError());
    }

    slog("entity system initialized");
    atexit(IMG_Quit);
    // atexit(gf2d_sprite_close);
}
/*eol@eof*/
